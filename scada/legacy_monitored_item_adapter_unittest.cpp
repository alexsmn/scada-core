#include "scada/legacy_monitored_item_adapter.h"

#include "base/test/awaitable_test.h"
#include "base/test/test_executor.h"
#include "scada/attribute_service.h"
#include "scada/service_context.h"

#include <boost/asio/redirect_error.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/this_coro.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <deque>
#include <mutex>

namespace scada {
namespace {

using namespace testing;

class FakeMonitoredItemSubscription final : public MonitoredItemSubscription {
 public:
  struct State {
    std::mutex mutex;
    std::vector<MonitoredItemCreateRequest> add_requests;
    std::vector<MonitoredItemId> remove_item_ids;
    std::deque<StatusOr<std::vector<MonitoredItemNotification>>> pending_reads;
    std::shared_ptr<boost::asio::steady_timer> read_waiter;
    Status add_status = StatusCode::Good;
    Status close_status = StatusCode::Good;
    MonitoredItemId next_item_id = 100;
    bool closed = false;
  };

  explicit FakeMonitoredItemSubscription(std::shared_ptr<State> state)
      : state_{std::move(state)} {}

  Awaitable<std::vector<MonitoredItemCreateResult>> AddItems(
      std::vector<MonitoredItemCreateRequest> requests) override {
    std::vector<MonitoredItemCreateResult> results;
    results.reserve(requests.size());

    std::lock_guard lock{state_->mutex};
    for (const auto& request : requests) {
      state_->add_requests.push_back(request);
      results.push_back({
          .item_id = state_->add_status ? state_->next_item_id++ : 0,
          .client_handle = request.client_handle,
          .status = state_->add_status,
      });
    }
    co_return results;
  }

  Awaitable<std::vector<Status>> RemoveItems(
      std::span<const MonitoredItemId> item_ids) override {
    std::lock_guard lock{state_->mutex};
    state_->remove_item_ids.insert(state_->remove_item_ids.end(),
                                   item_ids.begin(), item_ids.end());
    co_return std::vector<Status>(item_ids.size(), StatusCode::Good);
  }

  Awaitable<StatusOr<std::vector<MonitoredItemNotification>>> ReadNext(
      std::size_t max_count) override {
    for (;;) {
      {
        std::lock_guard lock{state_->mutex};
        if (state_->closed)
          co_return state_->close_status;

        if (!state_->pending_reads.empty()) {
          auto result = std::move(state_->pending_reads.front());
          state_->pending_reads.pop_front();
          co_return result;
        }
      }

      auto executor = co_await boost::asio::this_coro::executor;
      auto timer = std::make_shared<boost::asio::steady_timer>(executor);
      timer->expires_at((boost::asio::steady_timer::time_point::max)());
      {
        std::lock_guard lock{state_->mutex};
        if (state_->closed)
          co_return state_->close_status;
        if (!state_->pending_reads.empty())
          continue;
        state_->read_waiter = timer;
      }

      boost::system::error_code error;
      co_await timer->async_wait(
          boost::asio::redirect_error(boost::asio::use_awaitable, error));
      {
        std::lock_guard lock{state_->mutex};
        if (state_->read_waiter == timer)
          state_->read_waiter.reset();
      }
    }
  }

  void Close(Status status) override {
    std::lock_guard lock{state_->mutex};
    state_->closed = true;
    state_->close_status = std::move(status);
    if (state_->read_waiter) {
      state_->read_waiter->cancel();
      state_->read_waiter.reset();
    }
  }

 private:
  std::shared_ptr<State> state_;
};

class FakeMonitoredItemService final : public MonitoredItemService {
 public:
  explicit FakeMonitoredItemService(
      std::shared_ptr<FakeMonitoredItemSubscription::State> subscription_state)
      : subscription_state_{std::move(subscription_state)} {}

  StatusOr<std::unique_ptr<MonitoredItemSubscription>> CreateSubscription(
      ServiceContext context,
      MonitoredItemSubscriptionOptions options) override {
    create_count++;
    create_context = std::move(context);
    create_options = options;
    return std::unique_ptr<MonitoredItemSubscription>{
        std::make_unique<FakeMonitoredItemSubscription>(subscription_state_)};
  }

  void PushRead(StatusOr<std::vector<MonitoredItemNotification>> result) {
    std::lock_guard lock{subscription_state_->mutex};
    subscription_state_->pending_reads.push_back(std::move(result));
    if (subscription_state_->read_waiter) {
      subscription_state_->read_waiter->cancel();
      subscription_state_->read_waiter.reset();
    }
  }

  std::vector<MonitoredItemCreateRequest> AddRequests() const {
    std::lock_guard lock{subscription_state_->mutex};
    return subscription_state_->add_requests;
  }

  std::vector<MonitoredItemId> RemoveItemIds() const {
    std::lock_guard lock{subscription_state_->mutex};
    return subscription_state_->remove_item_ids;
  }

  std::shared_ptr<FakeMonitoredItemSubscription::State> subscription_state_;
  int create_count = 0;
  ServiceContext create_context;
  MonitoredItemSubscriptionOptions create_options;
};

DataValue GoodValue(Int32 value) {
  return MakeReadResult(value);
}

TEST(LegacyMonitoredItemAdapter, ReusesOneSubscriptionForMultipleLegacyItems) {
  TestExecutor executor;
  auto subscription_state =
      std::make_shared<FakeMonitoredItemSubscription::State>();
  FakeMonitoredItemService service{subscription_state};
  LegacyMonitoredItemAdapter pump{
      executor, service, {.max_pending_notifications = 7, .max_batch_size = 3}};
  auto item1 = pump.CreateMonitoredItem(
      {.node_id = NodeId{1, 1}, .attribute_id = AttributeId::Value}, {});
  auto item2 = pump.CreateMonitoredItem(
      {.node_id = NodeId{2, 1}, .attribute_id = AttributeId::Value}, {});

  item1->Subscribe(DataChangeHandler{[](const DataValue&) {}});
  item2->Subscribe(DataChangeHandler{[](const DataValue&) {}});
  Drain(executor);

  EXPECT_EQ(service.create_count, 1);
  EXPECT_EQ(service.create_options.max_pending_notifications, 7u);
  EXPECT_EQ(service.create_options.max_batch_size, 3u);
  const auto add_requests = service.AddRequests();
  ASSERT_THAT(add_requests, SizeIs(2));
  EXPECT_EQ(add_requests[0].item_to_monitor.node_id, (NodeId{1, 1}));
  EXPECT_EQ(add_requests[0].client_handle, 1u);
  EXPECT_EQ(add_requests[1].item_to_monitor.node_id, (NodeId{2, 1}));
  EXPECT_EQ(add_requests[1].client_handle, 2u);
}

TEST(LegacyMonitoredItemAdapter, FansOutDataChangesByClientHandle) {
  TestExecutor executor;
  auto subscription_state =
      std::make_shared<FakeMonitoredItemSubscription::State>();
  FakeMonitoredItemService service{subscription_state};
  LegacyMonitoredItemAdapter pump{executor, service};
  auto item1 = pump.CreateMonitoredItem(
      {.node_id = NodeId{1, 1}, .attribute_id = AttributeId::Value}, {});
  auto item2 = pump.CreateMonitoredItem(
      {.node_id = NodeId{2, 1}, .attribute_id = AttributeId::Value}, {});
  std::vector<Int32> values1;
  std::vector<Int32> values2;

  item1->Subscribe(DataChangeHandler{[&](const DataValue& value) {
    values1.push_back(value.value.as_int32());
  }});
  item2->Subscribe(DataChangeHandler{[&](const DataValue& value) {
    values2.push_back(value.value.as_int32());
  }});
  service.PushRead(std::vector<MonitoredItemNotification>{
      DataChangeNotification{
          .item_id = 101, .client_handle = 2, .value = GoodValue(22)},
      EventNotification{.item_id = 100, .client_handle = 1},
      DataChangeNotification{
          .item_id = 100, .client_handle = 1, .value = GoodValue(11)},
      DataChangeNotification{
          .item_id = 999, .client_handle = 99, .value = GoodValue(99)},
  });
  Drain(executor);

  EXPECT_THAT(values1, ElementsAre(11));
  EXPECT_THAT(values2, ElementsAre(22));
}

TEST(LegacyMonitoredItemAdapter, DestroyingItemRemovesSubscriptionItem) {
  TestExecutor executor;
  auto subscription_state =
      std::make_shared<FakeMonitoredItemSubscription::State>();
  FakeMonitoredItemService service{subscription_state};
  LegacyMonitoredItemAdapter pump{executor, service};
  auto item = pump.CreateMonitoredItem(
      {.node_id = NodeId{1, 1}, .attribute_id = AttributeId::Value}, {});
  item->Subscribe(DataChangeHandler{[](const DataValue&) {}});
  Drain(executor);
  ASSERT_THAT(service.AddRequests(), SizeIs(1));

  item.reset();
  Drain(executor);

  EXPECT_THAT(service.RemoveItemIds(), ElementsAre(100u));
}

TEST(LegacyMonitoredItemAdapter, FailedAddDoesNotDeliverStaleNotifications) {
  TestExecutor executor;
  auto subscription_state =
      std::make_shared<FakeMonitoredItemSubscription::State>();
  subscription_state->add_status = StatusCode::Bad_WrongNodeId;
  FakeMonitoredItemService service{subscription_state};
  LegacyMonitoredItemAdapter pump{executor, service};
  auto item = pump.CreateMonitoredItem(
      {.node_id = NodeId{1, 1}, .attribute_id = AttributeId::Value}, {});
  std::vector<Int32> values;

  item->Subscribe(DataChangeHandler{[&](const DataValue& value) {
    values.push_back(value.value.as_int32());
  }});
  service.PushRead(std::vector<MonitoredItemNotification>{
      DataChangeNotification{
          .item_id = 0, .client_handle = 1, .value = GoodValue(11)},
  });
  Drain(executor);

  EXPECT_THAT(values, IsEmpty());
  EXPECT_THAT(service.RemoveItemIds(), IsEmpty());
}

TEST(LegacyMonitoredItemAdapter, CloseClosesUnderlyingSubscription) {
  TestExecutor executor;
  auto subscription_state =
      std::make_shared<FakeMonitoredItemSubscription::State>();
  FakeMonitoredItemService service{subscription_state};
  LegacyMonitoredItemAdapter pump{executor, service};
  auto item = pump.CreateMonitoredItem(
      {.node_id = NodeId{1, 1}, .attribute_id = AttributeId::Value}, {});
  item->Subscribe(DataChangeHandler{[](const DataValue&) {}});
  Drain(executor);

  pump.Close(StatusCode::Bad_ServerWasShutDown);
  Drain(executor);

  EXPECT_EQ(subscription_state->close_status,
            StatusCode::Bad_ServerWasShutDown);
}

}  // namespace
}  // namespace scada
