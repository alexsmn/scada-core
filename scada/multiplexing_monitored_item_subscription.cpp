#include "scada/multiplexing_monitored_item_subscription.h"

#include "scada/attribute_ids.h"
#include "scada/monitored_item_service.h"
#include "scada/monitored_item_subscription_pump.h"
#include "scada/serving_gate.h"

#include <algorithm>
#include <boost/asio/redirect_error.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/this_coro.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <deque>
#include <mutex>
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
      ServingGate* gate)
      : state_{std::make_shared<State>(std::move(executor),
                                       std::move(router),
                                       options)} {
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

  Awaitable<StatusOr<std::vector<MonitoredItemNotification>>> ReadNext(
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
    std::vector<std::pair<MonitoredItemService*, MonitoredItemId>> children;
  };

  struct State {
    State(AnyExecutor executor,
          MonitoredItemRouter router,
          MonitoredItemSubscriptionOptions options)
        : executor{std::move(executor)},
          router{std::move(router)},
          options{options} {}

    AnyExecutor executor;
    MonitoredItemRouter router;
    MonitoredItemSubscriptionOptions options;

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
    std::deque<MonitoredItemNotification> pending;
    std::shared_ptr<boost::asio::steady_timer> read_waiter;
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
  // use. Must be called with the state mutex held.
  static MonitoredItemSubscriptionPump* EnsurePump(
      const std::shared_ptr<State>& state,
      MonitoredItemService* service) {
    auto& pump = state->pumps[service];
    if (!pump) {
      std::weak_ptr<State> weak_state = state;
      pump = std::make_unique<MonitoredItemSubscriptionPump>(
          state->executor, *service, state->options,
          [weak_state](std::vector<MonitoredItemNotification> notifications) {
            for (auto& notification : notifications) {
              PushNotification(weak_state, std::move(notification));
            }
          },
          [](Status /*status*/) {});
      if (!pump->Start().good()) {
        state->pumps.erase(service);
        return nullptr;
      }
    }
    return pump.get();
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
          Item{.client_handle = request.client_handle};
    }

    bool any_added = false;
    for (const auto& route : routes) {
      MonitoredItemSubscriptionPump* pump = nullptr;
      {
        std::lock_guard lock{state_->mutex};
        if (state_->closed) {
          break;
        }
        pump = EnsurePump(state_, route.service);
      }
      if (!pump) {
        continue;
      }

      std::vector<MonitoredItemCreateRequest> child_requests;
      child_requests.push_back({.item_to_monitor = route.value_id,
                                .parameters = request.parameters,
                                .client_handle = router_item_id});
      auto child_results = co_await pump->AddItems(std::move(child_requests));
      if (!child_results.empty() && child_results.front().status.good()) {
        std::lock_guard lock{state_->mutex};
        if (auto it = state_->items.find(router_item_id);
            it != state_->items.end()) {
          it->second.children.emplace_back(route.service,
                                           child_results.front().item_id);
          any_added = true;
        }
      }
    }

    if (!any_added) {
      std::lock_guard lock{state_->mutex};
      state_->items.erase(router_item_id);
      co_return MonitoredItemCreateResult{
          .client_handle = request.client_handle,
          .status = NoServiceStatus(request.item_to_monitor)};
    }

    co_return MonitoredItemCreateResult{.item_id = router_item_id,
                                        .client_handle = request.client_handle,
                                        .status = StatusCode::Good};
  }

  Awaitable<Status> RemoveItem(MonitoredItemId item_id) {
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
    ServingGate* gate) {
  return std::unique_ptr<MonitoredItemSubscription>{
      std::make_unique<MultiplexingMonitoredItemSubscription>(
          std::move(executor), std::move(router), options, gate)};
}

}  // namespace scada
