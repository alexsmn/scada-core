#include "scada/multiplexing_monitored_item_subscription.h"

#include "base/awaitable.h"
#include "scada/attribute_ids.h"
#include "scada/co_result.h"
#include "scada/monitored_item_service.h"
#include "scada/monitored_item_subscription_pump.h"
#include "scada/serving_gate.h"

#include <algorithm>
#include <boost/asio/post.hpp>
#include <boost/asio/redirect_error.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/this_coro.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <deque>
#include <mutex>
#include <optional>
#include <unordered_map>
#include <utility>
#include <vector>

namespace scada {
namespace {

// Returns the per-item create status when no backend owns the node.
StatusCode NoServiceStatus(const ReadValueId& value_id) {
  return value_id.attribute_id == AttributeId::Value ||
                 value_id.attribute_id == AttributeId::EventNotifier
             ? StatusCode::Bad_WrongNodeId
             : StatusCode::Bad_WrongAttributeId;
}

// Subscription that routes items to backend services through a
// `MonitoredItemRouter` and merges their notification streams. Each backend is
// driven by its own `MonitoredItemSubscriptionPump`.
class MultiplexingMonitoredItemSubscription final
    : public MonitoredItemSubscription {
 public:
  MultiplexingMonitoredItemSubscription(
      AnyExecutor executor,
      MonitoredItemRouter router,
      MonitoredItemSubscriptionOptions options,
      ServingGate* gate,
      ServiceContext context,
      std::chrono::milliseconds rebind_delay)
      : state_{std::make_shared<State>(std::move(executor),
                                       std::move(router),
                                       options,
                                       std::move(context),
                                       rebind_delay)} {
    if (gate) {
      state_->gate_view = gate->view();
      // Registered while blocked delivery is off: a new subscription cannot be
      // created while the gate is blocked (CreateSubscription is refused), so
      // it never misses a sweep. The registration auto-unregisters when
      // `state_` (and thus its member) is destroyed.
      std::weak_ptr<State> weak_state = state_;
      state_->registration =
          gate->Register([weak_state](StatusCode blocked_status) {
            SweepBlocked(weak_state, blocked_status);
          });
    }
  }

  ~MultiplexingMonitoredItemSubscription() override {
    Close(StatusCode::Bad_Disconnected);
  }

  Awaitable<std::vector<MonitoredItemCreateResult>> AddItems(
      std::vector<MonitoredItemCreateRequest> requests) override {
    std::vector<MonitoredItemCreateResult> results;
    results.reserve(requests.size());
    for (auto& request : requests) {
      results.emplace_back(co_await AddItem(std::move(request)));
    }
    co_return results;
  }

  Awaitable<std::vector<Status>> RemoveItems(
      std::span<const MonitoredItemId> item_ids) override {
    std::vector<Status> results;
    results.reserve(item_ids.size());
    for (auto item_id : item_ids) {
      results.emplace_back(co_await RemoveItem(item_id));
    }
    co_return results;
  }

  CoStatusOr<std::vector<MonitoredItemNotification>> ReadNext(
      std::size_t max_count) override {
    if (max_count == 0) {
      co_return std::vector<MonitoredItemNotification>{};
    }

    for (;;) {
      std::vector<MonitoredItemNotification> result;
      {
        std::lock_guard lock{state_->mutex};
        if (state_->closed) {
          co_return state_->close_status;
        }
        const std::size_t count =
            std::min({max_count, state_->options.max_batch_size,
                      state_->pending.size()});
        if (count != 0) {
          result.reserve(count);
          for (std::size_t i = 0; i < count; ++i) {
            result.emplace_back(std::move(state_->pending.front()));
            state_->pending.pop_front();
          }
          co_return result;
        }
        if (state_->read_waiter) {
          co_return Status{StatusCode::Bad_ObjectIsBusy};
        }
      }

      auto executor = co_await boost::asio::this_coro::executor;
      auto timer = std::make_shared<boost::asio::steady_timer>(executor);
      timer->expires_at((boost::asio::steady_timer::time_point::max)());
      {
        std::lock_guard lock{state_->mutex};
        if (state_->closed) {
          co_return state_->close_status;
        }
        if (!state_->pending.empty()) {
          continue;
        }
        state_->read_waiter = timer;
      }

      boost::system::error_code error;
      co_await timer->async_wait(
          boost::asio::redirect_error(boost::asio::use_awaitable, error));

      {
        std::lock_guard lock{state_->mutex};
        if (state_->read_waiter == timer) {
          state_->read_waiter.reset();
        }
      }
    }
  }

  void Close(Status status) override {
    std::vector<std::unique_ptr<MonitoredItemSubscriptionPump>> pumps;
    {
      std::lock_guard lock{state_->mutex};
      if (state_->closed) {
        return;
      }
      state_->closed = true;
      state_->close_status = std::move(status);
      state_->items.clear();
      state_->pending.clear();
      for (auto& [service, pump] : state_->pumps) {
        pumps.push_back(std::move(pump));
      }
      state_->pumps.clear();
      if (state_->read_waiter) {
        state_->read_waiter->cancel();
        state_->read_waiter.reset();
      }
      if (state_->rebind_timer) {
        state_->rebind_timer->cancel();
        state_->rebind_timer.reset();
      }
    }
    for (auto& pump : pumps) {
      if (pump) {
        pump->Close(StatusCode::Bad_Disconnected);
      }
    }
  }

 private:
  struct Item {
    std::uint32_t client_handle = 0;
    // What the client asked for, kept so the item can be routed again after
    // the backend serving it goes away.
    ReadValueId value_id;
    MonitoringParameters params;
    std::vector<std::pair<MonitoredItemService*, MonitoredItemId>> children;
    // A backend this item was bound to failed; RebindLoop routes it again.
    bool needs_rebind = false;
  };

  struct State {
    State(AnyExecutor executor,
          MonitoredItemRouter router,
          MonitoredItemSubscriptionOptions options,
          ServiceContext context,
          std::chrono::milliseconds rebind_delay)
        : executor{std::move(executor)},
          router{std::move(router)},
          options{options},
          context{std::move(context)},
          rebind_delay{rebind_delay} {}

    AnyExecutor executor;
    MonitoredItemRouter router;
    MonitoredItemSubscriptionOptions options;
    // The context this subscription was created with, handed to each backend's
    // CreateSubscription so its spans link back to the caller's trace.
    ServiceContext context;
    // The first wait before routing a lost item again (see RebindLoop).
    const std::chrono::milliseconds rebind_delay;

    // Optional serving gate: while blocked, delivered notifications are
    // rewritten to its blocked status and `registration` drives the item sweep.
    // `gate_view` is a lifetime-safe read handle (a default-constructed view
    // means "no gate": always allowed), so the notification-time read stays
    // valid even when this State outlives the gate. `registration` unregisters
    // the sweep callback and is likewise safe to destroy after the gate.
    ServingGate::View gate_view;
    ServingGate::Registration registration;

    std::mutex mutex;
    MonitoredItemId next_item_id = 1;
    std::unordered_map<MonitoredItemId, Item> items;
    std::unordered_map<MonitoredItemService*,
                       std::unique_ptr<MonitoredItemSubscriptionPump>>
        pumps;
    // The id of the pump last made for each backend (see EnsurePump).
    std::unordered_map<MonitoredItemService*, std::uint64_t> pump_ids;
    std::uint64_t next_pump_id = 1;
    std::deque<MonitoredItemNotification> pending;
    std::shared_ptr<boost::asio::steady_timer> read_waiter;
    // Paces RebindLoop while it runs; cancelled by Close.
    std::shared_ptr<boost::asio::steady_timer> rebind_timer;
    bool rebind_running = false;
    bool overflow_reported = false;
    bool closed = false;
    Status close_status = StatusCode::Bad_Disconnected;
  };

  // Backends are added with `client_handle` set to the router item id, so each
  // merged notification identifies the owning router item here.
  static void PushNotification(std::weak_ptr<State> weak_state,
                               MonitoredItemNotification notification) {
    auto state = weak_state.lock();
    if (!state) {
      return;
    }

    std::lock_guard lock{state->mutex};
    if (state->closed) {
      return;
    }

    auto remap = [&](MonitoredItemId& item_id, std::uint32_t& client_handle) {
      const auto router_item_id = static_cast<MonitoredItemId>(client_handle);
      auto it = state->items.find(router_item_id);
      item_id = router_item_id;
      client_handle = it != state->items.end() ? it->second.client_handle : 0;
    };

    if (auto* data_change =
            std::get_if<DataChangeNotification>(&notification)) {
      remap(data_change->item_id, data_change->client_handle);
    } else if (auto* event = std::get_if<EventNotification>(&notification)) {
      remap(event->item_id, event->client_handle);
    } else if (auto* item_status =
                   std::get_if<ItemStatusNotification>(&notification)) {
      remap(item_status->item_id, item_status->client_handle);
    }

    // While the serving gate is blocked, do not leak a live backend sample:
    // rewrite the notification's status to the blocked status so the client
    // sees the outage even for items that keep updating.
    if (!state->gate_view.allowed()) {
      const StatusCode blocked = state->gate_view.blocked_status();
      if (auto* data_change =
              std::get_if<DataChangeNotification>(&notification)) {
        data_change->value.status_code = blocked;
      } else if (auto* event = std::get_if<EventNotification>(&notification)) {
        event->status = blocked;
      } else if (auto* item_status =
                     std::get_if<ItemStatusNotification>(&notification)) {
        item_status->status = blocked;
      }
    }

    if (state->pending.size() >= state->options.max_pending_notifications) {
      if (!state->overflow_reported) {
        state->overflow_reported = true;
        state->pending.emplace_back(
            OverflowNotification{StatusCode::Bad_ObjectIsBusy});
      }
      return;
    }

    state->pending.emplace_back(std::move(notification));
    if (state->read_waiter) {
      state->read_waiter->cancel();
      state->read_waiter.reset();
    }
  }

  // Pushes `blocked_status` to every active item, so items whose backend value
  // does not change during the block still report the outage. Invoked by the
  // ServingGate on the transition into blocked (never with the gate lock held).
  // Mirrors PushNotification's queueing/wake, but sets item_id/client_handle
  // directly rather than remapping a backend client_handle.
  static void SweepBlocked(std::weak_ptr<State> weak_state,
                           StatusCode blocked_status) {
    auto state = weak_state.lock();
    if (!state) {
      return;
    }

    std::lock_guard lock{state->mutex};
    if (state->closed) {
      return;
    }

    for (const auto& [item_id, item] : state->items) {
      if (state->pending.size() >= state->options.max_pending_notifications) {
        if (!state->overflow_reported) {
          state->overflow_reported = true;
          state->pending.emplace_back(
              OverflowNotification{StatusCode::Bad_ObjectIsBusy});
        }
        break;
      }
      state->pending.emplace_back(
          ItemStatusNotification{.item_id = item_id,
                                 .client_handle = item.client_handle,
                                 .status = blocked_status});
    }

    if (state->read_waiter) {
      state->read_waiter->cancel();
      state->read_waiter.reset();
    }
  }

  // Returns the backend pump for `service`, creating and starting it on first
  // use, or null when the subscription is closed or the backend refuses a
  // subscription. Must be called WITHOUT the state mutex: starting a pump runs
  // its first read, which on an io_context thread can complete inline and
  // deliver notifications -- or a failure -- straight into handlers that take
  // the mutex. So the pump is made and started unlocked and published after;
  // if a concurrent caller published one first, theirs wins and ours closes.
  static MonitoredItemSubscriptionPump* EnsurePump(
      const std::shared_ptr<State>& state,
      MonitoredItemService* service) {
    std::uint64_t pump_id = 0;
    {
      std::lock_guard lock{state->mutex};
      if (state->closed) {
        return nullptr;
      }
      if (auto it = state->pumps.find(service); it != state->pumps.end()) {
        return it->second.get();
      }
      // Identifies this pump to its own error handler, which must not act on
      // a successor made for the same backend.
      pump_id = state->next_pump_id++;
    }

    std::weak_ptr<State> weak_state = state;
    auto pump = std::make_unique<MonitoredItemSubscriptionPump>(
        state->executor, *service, state->options,
        [weak_state](std::vector<MonitoredItemNotification> notifications) {
          for (auto& notification : notifications) {
            PushNotification(weak_state, std::move(notification));
          }
        },
        // Posted rather than run inline, so a failure never re-enters
        // whatever the failing read was called from.
        [weak_state, service, pump_id,
         executor = state->executor](Status status) {
          boost::asio::post(executor, [weak_state, service, pump_id,
                                       status = std::move(status)]() mutable {
            OnBackendFailed(weak_state, service, pump_id, std::move(status));
          });
        });
    if (!pump->Start(state->context).good()) {
      return nullptr;
    }

    std::unique_ptr<MonitoredItemSubscriptionPump> unused;
    MonitoredItemSubscriptionPump* result = nullptr;
    {
      std::lock_guard lock{state->mutex};
      if (state->closed) {
        unused = std::move(pump);
      } else if (auto it = state->pumps.find(service);
                 it != state->pumps.end()) {
        unused = std::move(pump);
        result = it->second.get();
      } else {
        state->pump_ids[service] = pump_id;
        result = pump.get();
        state->pumps.emplace(service, std::move(pump));
      }
    }
    return result;
  }

  // Queues `notification` for ReadNext, reporting one overflow when the queue
  // is full. Must be called with the state mutex held.
  static void EnqueueLocked(State& state,
                            MonitoredItemNotification notification) {
    if (state.pending.size() >= state.options.max_pending_notifications) {
      if (!state.overflow_reported) {
        state.overflow_reported = true;
        state.pending.emplace_back(
            OverflowNotification{StatusCode::Bad_ObjectIsBusy});
      }
      return;
    }
    state.pending.emplace_back(std::move(notification));
    if (state.read_waiter) {
      state.read_waiter->cancel();
      state.read_waiter.reset();
    }
  }

  // A backend's subscription failed — for an aggregated downstream, its
  // session dropped because the tier stopped or restarted. Drops the dead
  // pump, unbinds every item it carried, and schedules them to be routed
  // again. An item left with no backend at all is reported with `status`, so a
  // client sees the outage rather than a frozen last value.
  //
  // Until 2026-10-26 the pump's error handler did nothing: the dead pump stayed
  // in `pumps`, every item bound to it went silent with no status, and nothing
  // ever routed it again — an edge restart silenced the historian's collection
  // and every open display until they were restarted (backlog 854).
  static void OnBackendFailed(std::weak_ptr<State> weak_state,
                              MonitoredItemService* service,
                              std::uint64_t pump_id,
                              Status status) {
    auto state = weak_state.lock();
    if (!state) {
      return;
    }
    if (status.good()) {
      status = StatusCode::Bad_Disconnected;
    }

    std::unique_ptr<MonitoredItemSubscriptionPump> dead_pump;
    bool start_rebind = false;
    {
      std::lock_guard lock{state->mutex};
      if (state->closed) {
        return;
      }
      auto pump_it = state->pumps.find(service);
      // A later pump for the same backend may already have replaced this one.
      if (pump_it == state->pumps.end() ||
          state->pump_ids[service] != pump_id) {
        return;
      }
      dead_pump = std::move(pump_it->second);
      state->pumps.erase(pump_it);

      bool any_unbound = false;
      for (auto& [item_id, item] : state->items) {
        const auto removed = std::erase_if(
            item.children,
            [service](const auto& child) { return child.first == service; });
        if (removed == 0) {
          continue;
        }
        item.needs_rebind = true;
        any_unbound = true;
        if (item.children.empty()) {
          EnqueueLocked(*state, ItemStatusNotification{
                                    .item_id = item_id,
                                    .client_handle = item.client_handle,
                                    .status = status});
        }
      }
      if (any_unbound && !state->rebind_running) {
        state->rebind_running = true;
        start_rebind = true;
      }
    }
    // Outside the lock, both. Closing reaches into the backend. And spawning
    // must not hold the mutex: on an io_context thread co_spawn starts the
    // coroutine inline, and RebindLoop's first act is to take the same
    // mutex -- the proxy deadlocked exactly so the first time an edge was
    // restarted under this code (2026-10-26), while TestExecutor, which
    // always queues, never showed it.
    dead_pump.reset();
    if (start_rebind) {
      CoSpawn(state->executor, [weak_state = std::weak_ptr<State>{state}] {
        return RebindLoop(std::move(weak_state));
      });
    }
  }

  // Routes the items that lost a backend again, waiting `rebind_delay` before
  // the first pass and doubling up to kMaxRebindDelay while some stay unbound.
  // It polls rather than waiting for a "routes changed" signal because the
  // routing table is the caller's (an aggregating proxy's downstreams come and
  // go and reconnect on their own schedule), and asking it costs a lookup per
  // lost item.
  static Awaitable<void> RebindLoop(std::weak_ptr<State> weak_state) {
    std::chrono::milliseconds delay;
    {
      auto state = weak_state.lock();
      if (!state) {
        co_return;
      }
      delay = state->rebind_delay;
    }
    for (;;) {
      std::shared_ptr<boost::asio::steady_timer> timer;
      {
        auto state = weak_state.lock();
        if (!state) {
          co_return;
        }
        std::lock_guard lock{state->mutex};
        if (state->closed) {
          co_return;
        }
        timer = std::make_shared<boost::asio::steady_timer>(state->executor);
        timer->expires_after(delay);
        state->rebind_timer = timer;
      }
      boost::system::error_code error;
      co_await timer->async_wait(
          boost::asio::redirect_error(boost::asio::use_awaitable, error));

      auto state = weak_state.lock();
      if (!state) {
        co_return;
      }
      std::vector<MonitoredItemId> pending_items;
      {
        std::lock_guard lock{state->mutex};
        if (state->closed) {
          co_return;
        }
        for (const auto& [item_id, item] : state->items) {
          if (item.needs_rebind) {
            pending_items.push_back(item_id);
          }
        }
        if (pending_items.empty()) {
          state->rebind_running = false;
          state->rebind_timer.reset();
          co_return;
        }
      }

      for (MonitoredItemId item_id : pending_items) {
        std::optional<std::pair<ReadValueId, MonitoringParameters>> request;
        {
          std::lock_guard lock{state->mutex};
          if (state->closed) {
            co_return;
          }
          if (auto it = state->items.find(item_id);
              it != state->items.end() && it->second.needs_rebind) {
            request.emplace(it->second.value_id, it->second.params);
          }
        }
        if (!request) {
          continue;
        }
        std::vector<MonitoredItemRoute> routes =
            state->router(request->first, request->second);
        const bool all_bound = co_await BindRoutes(state, item_id, routes);
        std::lock_guard lock{state->mutex};
        if (auto it = state->items.find(item_id); it != state->items.end()) {
          it->second.needs_rebind = !all_bound;
        }
      }
      delay = std::min(delay * 2, kMaxRebindDelay);
    }
  }

  // Binds router item `router_item_id` to each of `routes` it is not already
  // bound to. Returns true when every route now carries the item and at least
  // one route was given.
  static Awaitable<bool> BindRoutes(
      const std::shared_ptr<State>& state,
      MonitoredItemId router_item_id,
      const std::vector<MonitoredItemRoute>& routes) {
    if (routes.empty()) {
      co_return false;
    }
    bool all_bound = true;
    for (const auto& route : routes) {
      MonitoredItemSubscriptionPump* pump = nullptr;
      MonitoringParameters params;
      {
        std::lock_guard lock{state->mutex};
        if (state->closed) {
          co_return false;
        }
        auto it = state->items.find(router_item_id);
        if (it == state->items.end()) {
          co_return false;
        }
        if (std::ranges::any_of(it->second.children, [&](const auto& child) {
              return child.first == route.service;
            })) {
          continue;
        }
        params = it->second.params;
      }
      pump = EnsurePump(state, route.service);
      if (!pump) {
        all_bound = false;
        continue;
      }

      std::vector<MonitoredItemCreateRequest> child_requests;
      child_requests.push_back({.item_to_monitor = route.value_id,
                                .parameters = std::move(params),
                                .client_handle = router_item_id});
      auto child_results = co_await pump->AddItems(std::move(child_requests));
      const bool added =
          !child_results.empty() && child_results.front().status.good();

      std::unique_ptr<MonitoredItemSubscriptionPump> dropped_pump;
      std::optional<MonitoredItemId> orphan_child;
      {
        std::lock_guard lock{state->mutex};
        auto it = state->items.find(router_item_id);
        if (added && it != state->items.end() && !state->closed) {
          it->second.children.emplace_back(route.service,
                                           child_results.front().item_id);
        } else {
          all_bound = false;
          if (added) {
            // The item was removed (or the subscription closed) while the
            // backend was adding it.
            orphan_child = child_results.front().item_id;
          }
          // A pump that carries nothing was made for this attempt, against a
          // backend that is not accepting items (a downstream still
          // reconnecting). Drop it so the next attempt starts a fresh
          // subscription instead of reusing one bound to the dead session.
          if (!added && !state->closed &&
              !CarriesItemsLocked(*state, route.service)) {
            if (auto pump_it = state->pumps.find(route.service);
                pump_it != state->pumps.end() &&
                pump_it->second.get() == pump) {
              dropped_pump = std::move(pump_it->second);
              state->pumps.erase(pump_it);
            }
          }
        }
      }
      if (orphan_child) {
        MonitoredItemSubscriptionPump* live_pump = nullptr;
        {
          std::lock_guard lock{state->mutex};
          if (auto pump_it = state->pumps.find(route.service);
              pump_it != state->pumps.end() && pump_it->second.get() == pump) {
            live_pump = pump;
          }
        }
        if (live_pump) {
          const MonitoredItemId child_ids[] = {*orphan_child};
          co_await live_pump->RemoveItems(child_ids);
        }
      }
    }
    co_return all_bound;
  }

  // Whether any item is bound to `service`. Must be called with the state
  // mutex held.
  static bool CarriesItemsLocked(const State& state,
                                 MonitoredItemService* service) {
    return std::ranges::any_of(state.items, [service](const auto& entry) {
      return std::ranges::any_of(
          entry.second.children,
          [service](const auto& child) { return child.first == service; });
    });
  }

  Awaitable<MonitoredItemCreateResult> AddItem(
      MonitoredItemCreateRequest request) {
    {
      std::lock_guard lock{state_->mutex};
      if (state_->closed) {
        co_return MonitoredItemCreateResult{
            .client_handle = request.client_handle,
            .status = state_->close_status};
      }
    }

    std::vector<MonitoredItemRoute> routes =
        state_->router(request.item_to_monitor, request.parameters);
    if (routes.empty()) {
      co_return MonitoredItemCreateResult{
          .client_handle = request.client_handle,
          .status = NoServiceStatus(request.item_to_monitor)};
    }

    MonitoredItemId router_item_id = 0;
    {
      std::lock_guard lock{state_->mutex};
      if (state_->closed) {
        co_return MonitoredItemCreateResult{
            .client_handle = request.client_handle,
            .status = state_->close_status};
      }
      router_item_id = state_->next_item_id++;
      state_->items[router_item_id] =
          Item{.client_handle = request.client_handle,
               .value_id = request.item_to_monitor,
               .params = request.parameters};
    }

    // A fan-out item that some of its backends refused is created on the
    // rest, as before, and is not retried: nothing was lost.
    co_await BindRoutes(state_, router_item_id, routes);

    {
      std::lock_guard lock{state_->mutex};
      auto it = state_->items.find(router_item_id);
      if (it == state_->items.end() || it->second.children.empty()) {
        if (it != state_->items.end()) {
          state_->items.erase(it);
        }
        co_return MonitoredItemCreateResult{
            .client_handle = request.client_handle,
            .status = NoServiceStatus(request.item_to_monitor)};
      }
    }

    co_return MonitoredItemCreateResult{.item_id = router_item_id,
                                        .client_handle = request.client_handle,
                                        .status = StatusCode::Good};
  }

  CoStatus RemoveItem(MonitoredItemId item_id) {
    std::vector<std::pair<MonitoredItemService*, MonitoredItemId>> children;
    {
      std::lock_guard lock{state_->mutex};
      auto it = state_->items.find(item_id);
      if (it == state_->items.end()) {
        co_return Status{StatusCode::Bad_MonitoredItemIdInvalid};
      }
      children = std::move(it->second.children);
      state_->items.erase(it);
    }

    for (auto& [service, child_item_id] : children) {
      MonitoredItemSubscriptionPump* pump = nullptr;
      {
        std::lock_guard lock{state_->mutex};
        if (auto it = state_->pumps.find(service); it != state_->pumps.end()) {
          pump = it->second.get();
        }
      }
      if (pump) {
        const MonitoredItemId child_ids[] = {child_item_id};
        co_await pump->RemoveItems(child_ids);
      }
    }

    co_return Status{StatusCode::Good};
  }

  const std::shared_ptr<State> state_;
};

}  // namespace

StatusOr<std::unique_ptr<MonitoredItemSubscription>>
MakeMultiplexingMonitoredItemSubscription(
    AnyExecutor executor,
    MonitoredItemRouter router,
    MonitoredItemSubscriptionOptions options,
    ServingGate* gate,
    ServiceContext context,
    std::chrono::milliseconds rebind_delay) {
  return std::unique_ptr<MonitoredItemSubscription>{
      std::make_unique<MultiplexingMonitoredItemSubscription>(
          std::move(executor), std::move(router), options, gate,
          std::move(context), rebind_delay)};
}

}  // namespace scada
