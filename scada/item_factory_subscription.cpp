#include "scada/item_factory_subscription.h"

#include "base/callback_awaitable.h"
#include "scada/attribute_ids.h"

#include <algorithm>
#include <boost/asio/this_coro.hpp>
#include <deque>
#include <functional>
#include <mutex>
#include <utility>

namespace scada {
namespace {

// Subscription that creates each monitored item through a
// `MonitoredItemFactory` and fans the items' callbacks into a single
// notification stream consumed via `ReadNext`.
class ItemFactorySubscription final : public MonitoredItemSubscription {
 public:
  ItemFactorySubscription(MonitoredItemFactory factory,
                          MonitoredItemSubscriptionOptions options)
      : state_{std::make_shared<State>(std::move(factory), options)} {}

  ~ItemFactorySubscription() override { Close(StatusCode::Bad_Disconnected); }

  Awaitable<std::vector<MonitoredItemCreateResult>> AddItems(
      std::vector<MonitoredItemCreateRequest> requests) override {
    std::vector<MonitoredItemCreateResult> results;
    results.reserve(requests.size());

    for (auto& request : requests) {
      results.emplace_back(AddItem(std::move(request)));
    }

    co_return results;
  }

  Awaitable<std::vector<Status>> RemoveItems(
      std::span<const MonitoredItemId> item_ids) override {
    std::vector<Status> results;
    results.reserve(item_ids.size());

    std::lock_guard lock{state_->mutex};
    for (auto item_id : item_ids) {
      if (item_id == 0 || item_id > state_->items.size() ||
          !state_->items[item_id - 1].monitored_item) {
        results.emplace_back(StatusCode::Bad_MonitoredItemIdInvalid);
        continue;
      }

      state_->items[item_id - 1].monitored_item.reset();
      results.emplace_back(StatusCode::Good);
    }

    co_return results;
  }

  Awaitable<StatusOr<std::vector<MonitoredItemNotification>>> ReadNext(
      std::size_t max_count) override {
    if (max_count == 0) {
      co_return std::vector<MonitoredItemNotification>{};
    }

    // Keep the shared state alive for the duration of this coroutine: it
    // suspends below and may be resumed after the owning subscription (and
    // thus the `state_` member) has been destroyed.
    const auto state = state_;
    const AnyExecutor executor = co_await boost::asio::this_coro::executor;

    for (;;) {
      {
        std::lock_guard lock{state->mutex};
        if (state->closed) {
          co_return state->close_status;
        }

        const std::size_t count = std::min(
            {max_count, state->options.max_batch_size, state->pending.size()});
        if (count != 0) {
          std::vector<MonitoredItemNotification> result;
          result.reserve(count);
          for (std::size_t i = 0; i < count; ++i) {
            result.emplace_back(std::move(state->pending.front()));
            state->pending.pop_front();
          }
          co_return result;
        }

        if (state->read_waiter) {
          co_return Status{StatusCode::Bad_ObjectIsBusy};
        }
      }

      // Park until a notification is pushed or the subscription is closed.
      // The wake is delivered through the executor rather than a
      // steady_timer cancellation, so this resumes deterministically on any
      // executor — including test executors that have no timer reactor.
      co_await CallbackToAwaitable<>(executor, [&state](auto wake) {
        std::lock_guard lock{state->mutex};
        if (state->closed || !state->pending.empty()) {
          // Data (or closure) is already available: wake immediately to
          // re-check instead of parking.
          wake();
          return;
        }
        state->read_waiter = [wake = std::move(wake)]() mutable { wake(); };
      });

      {
        std::lock_guard lock{state->mutex};
        state->read_waiter = nullptr;
      }
    }
  }

  void Close(Status status) override {
    std::function<void()> wake;
    {
      std::lock_guard lock{state_->mutex};
      if (state_->closed) {
        return;
      }

      state_->closed = true;
      state_->close_status = std::move(status);
      state_->items.clear();
      state_->pending.clear();
      wake = std::move(state_->read_waiter);
      state_->read_waiter = nullptr;
    }

    if (wake) {
      wake();
    }
  }

 private:
  struct Item {
    std::shared_ptr<MonitoredItem> monitored_item;
    std::uint32_t client_handle = 0;
  };

  struct State {
    State(MonitoredItemFactory factory,
          MonitoredItemSubscriptionOptions options)
        : factory{std::move(factory)}, options{options} {}

    MonitoredItemFactory factory;
    MonitoredItemSubscriptionOptions options;
    std::mutex mutex;
    std::vector<Item> items;
    std::deque<MonitoredItemNotification> pending;
    // Wake callback for a `ReadNext` coroutine parked waiting for data. The
    // callback resumes that coroutine through its executor, so no timer
    // reactor is required (works on deterministic test executors too).
    std::function<void()> read_waiter;
    bool overflow_reported = false;
    bool closed = false;
    Status close_status = StatusCode::Bad_Disconnected;
  };

  static void PushNotification(std::weak_ptr<State> weak_state,
                               MonitoredItemNotification notification) {
    auto state = weak_state.lock();
    if (!state) {
      return;
    }

    std::function<void()> wake;
    {
      std::lock_guard lock{state->mutex};
      if (state->closed) {
        return;
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
      wake = std::move(state->read_waiter);
      state->read_waiter = nullptr;
    }

    if (wake) {
      wake();
    }
  }

  MonitoredItemCreateResult AddItem(MonitoredItemCreateRequest request) {
    {
      std::lock_guard lock{state_->mutex};
      if (state_->closed) {
        return {.client_handle = request.client_handle,
                .status = state_->close_status};
      }
    }

    auto monitored_item =
        state_->factory(request.item_to_monitor, request.parameters);
    if (!monitored_item) {
      const auto status =
          request.item_to_monitor.attribute_id == AttributeId::Value ||
                  request.item_to_monitor.attribute_id ==
                      AttributeId::EventNotifier
              ? StatusCode::Bad_WrongNodeId
              : StatusCode::Bad_WrongAttributeId;
      return {.client_handle = request.client_handle, .status = status};
    }

    MonitoredItemId item_id = 0;
    {
      std::lock_guard lock{state_->mutex};
      item_id = static_cast<MonitoredItemId>(state_->items.size() + 1);
      state_->items.push_back({.monitored_item = monitored_item,
                               .client_handle = request.client_handle});
    }

    const auto client_handle = request.client_handle;
    std::weak_ptr<State> weak_state = state_;
    if (request.item_to_monitor.attribute_id == AttributeId::EventNotifier) {
      monitored_item->Subscribe(
          EventHandler{[weak_state, item_id, client_handle](
                           const Status& status, const std::any& event) {
            PushNotification(weak_state,
                             EventNotification{.item_id = item_id,
                                               .client_handle = client_handle,
                                               .status = status,
                                               .event = event});
          }});
    } else {
      monitored_item->Subscribe(DataChangeHandler{
          [weak_state, item_id, client_handle](const DataValue& value) {
            PushNotification(weak_state, DataChangeNotification{
                                             .item_id = item_id,
                                             .client_handle = client_handle,
                                             .value = value});
          }});
    }

    return {.item_id = item_id,
            .client_handle = request.client_handle,
            .status = StatusCode::Good};
  }

  const std::shared_ptr<State> state_;
};

}  // namespace

StatusOr<std::unique_ptr<MonitoredItemSubscription>>
MakeItemFactorySubscription(MonitoredItemFactory factory,
                            MonitoredItemSubscriptionOptions options) {
  return std::unique_ptr<MonitoredItemSubscription>{
      std::make_unique<ItemFactorySubscription>(std::move(factory), options)};
}

}  // namespace scada
