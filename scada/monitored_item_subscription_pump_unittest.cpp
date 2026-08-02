#include "scada/monitored_item_subscription_pump.h"

#include "base/test/awaitable_test.h"
#include "base/test/test_executor.h"
#include "scada/co_result.h"
#include "scada/service_context.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <deque>

namespace scada {
namespace {

using namespace testing;

class FakeMonitoredItemSubscription final : public MonitoredItemSubscription {
 public:
  struct State {
    std::vector<MonitoredItemCreateRequest> add_requests;
    std::vector<MonitoredItemCreateResult> add_results;
    std::vector<MonitoredItemId> remove_item_ids;
    std::vector<Status> remove_results;
    std::deque<StatusOr<std::vector<MonitoredItemNotification>>> read_results;
    std::vector<std::size_t> read_next_max_counts;
    Status close_status = StatusCode::Good;
  };

  explicit FakeMonitoredItemSubscription(std::shared_ptr<State> state)
      : state_{std::move(state)} {}

  Awaitable<std::vector<MonitoredItemCreateResult>> AddItems(
      std::vector<MonitoredItemCreateRequest> requests) override {
    state_->add_requests = std::move(requests);
    co_return state_->add_results;
  }

  Awaitable<std::vector<Status>> RemoveItems(
      std::span<const MonitoredItemId> item_ids) override {
    state_->remove_item_ids.assign(item_ids.begin(), item_ids.end());
    co_return state_->remove_results;
  }

  CoStatusOr<std::vector<MonitoredItemNotification>> ReadNext(
      std::size_t max_count) override {
    state_->read_next_max_counts.push_back(max_count);
    if (state_->read_results.empty()) {
      co_return Status{StatusCode::Bad_Disconnected};
    }

    StatusOr<std::vector<MonitoredItemNotification>> result =
        std::move(state_->read_results.front());
    state_->read_results.pop_front();
    co_return result;
  }

  void Close(Status status) override { state_->close_status = status; }

 private:
  std::shared_ptr<State> state_;
};

class FakeMonitoredItemService final : public MonitoredItemService {
 public:
  explicit FakeMonitoredItemService(
      StatusOr<std::unique_ptr<MonitoredItemSubscription>> subscription_result)
      : subscription_result{std::move(subscription_result)} {}

  StatusOr<std::unique_ptr<MonitoredItemSubscription>> CreateSubscription(
      ServiceContext context,
      MonitoredItemSubscriptionOptions options) override {
    create_context = std::move(context);
    create_options = options;
    return std::move(subscription_result);
  }

  StatusOr<std::unique_ptr<MonitoredItemSubscription>> subscription_result;
  ServiceContext create_context;
  MonitoredItemSubscriptionOptions create_options;
};

TEST(MonitoredItemSubscriptionPump, StartReturnsSubscriptionFailure) {
  TestExecutor executor;
  FakeMonitoredItemService service{
      StatusOr<std::unique_ptr<MonitoredItemSubscription>>{
          StatusCode::Bad_Disconnected}};
  std::vector<std::vector<MonitoredItemNotification>> notification_batches;
  std::vector<Status> errors;

  MonitoredItemSubscriptionPump pump{
      executor,
      service,
      {},
      [&](std::vector<MonitoredItemNotification> notifications) {
        notification_batches.emplace_back(std::move(notifications));
      },
      [&](Status status) { errors.emplace_back(std::move(status)); }};

  EXPECT_EQ(pump.Start(), StatusCode::Bad_Disconnected);
  Drain(executor);

  EXPECT_THAT(notification_batches, IsEmpty());
  EXPECT_THAT(errors, IsEmpty());
}

TEST(MonitoredItemSubscriptionPump, AddItemsBeforeStartFailsPerItem) {
  TestExecutor executor;
  FakeMonitoredItemService service{
      StatusOr<std::unique_ptr<MonitoredItemSubscription>>{
          StatusCode::Bad_Disconnected}};
  MonitoredItemSubscriptionPump pump{
      executor,
      service,
      {},
      [](std::vector<MonitoredItemNotification>) {},
      [](Status) {}};
  std::vector<MonitoredItemCreateRequest> requests{
      {.item_to_monitor = {.node_id = NodeId{1, 1}}, .client_handle = 11},
      {.item_to_monitor = {.node_id = NodeId{2, 1}}, .client_handle = 22}};

  std::vector<MonitoredItemCreateResult> results =
      WaitAwaitable(executor, pump.AddItems(std::move(requests)));

  ASSERT_THAT(results, SizeIs(2));
  EXPECT_EQ(results[0].client_handle, 11u);
  EXPECT_EQ(results[0].status, StatusCode::Bad_Disconnected);
  EXPECT_EQ(results[1].client_handle, 22u);
  EXPECT_EQ(results[1].status, StatusCode::Bad_Disconnected);
}

TEST(MonitoredItemSubscriptionPump, RemoveItemsBeforeStartFailsPerItem) {
  TestExecutor executor;
  FakeMonitoredItemService service{
      StatusOr<std::unique_ptr<MonitoredItemSubscription>>{
          StatusCode::Bad_Disconnected}};
  MonitoredItemSubscriptionPump pump{
      executor,
      service,
      {},
      [](std::vector<MonitoredItemNotification>) {},
      [](Status) {}};
  const std::array<MonitoredItemId, 2> item_ids{10, 20};

  std::vector<Status> results =
      WaitAwaitable(executor, pump.RemoveItems(item_ids));

  EXPECT_THAT(results, ElementsAre(Status{StatusCode::Bad_Disconnected},
                                   Status{StatusCode::Bad_Disconnected}));
}

TEST(MonitoredItemSubscriptionPump, ForwardsAddAndRemoveItems) {
  TestExecutor executor;
  std::shared_ptr<FakeMonitoredItemSubscription::State> subscription_state =
      std::make_shared<FakeMonitoredItemSubscription::State>();
  subscription_state->add_results.push_back({
      .item_id = 7,
      .client_handle = 42,
      .status = StatusCode::Good,
  });
  subscription_state->remove_results.push_back(StatusCode::Good);
  std::unique_ptr<FakeMonitoredItemSubscription> subscription =
      std::make_unique<FakeMonitoredItemSubscription>(subscription_state);
  FakeMonitoredItemService service{
      StatusOr<std::unique_ptr<MonitoredItemSubscription>>{
          std::move(subscription)}};
  MonitoredItemSubscriptionPump pump{
      executor,
      service,
      {},
      [](std::vector<MonitoredItemNotification>) {},
      [](Status) {}};

  ASSERT_TRUE(pump.Start());
  std::vector<MonitoredItemCreateRequest> requests{
      {.item_to_monitor = {.node_id = NodeId{1, 1}}, .client_handle = 42}};
  std::vector<MonitoredItemCreateResult> add_results =
      WaitAwaitable(executor, pump.AddItems(std::move(requests)));
  const std::array<MonitoredItemId, 1> item_ids{7};
  std::vector<Status> remove_results =
      WaitAwaitable(executor, pump.RemoveItems(item_ids));

  ASSERT_THAT(subscription_state->add_requests, SizeIs(1));
  EXPECT_EQ(subscription_state->add_requests[0].client_handle, 42u);
  EXPECT_EQ(subscription_state->add_requests[0].item_to_monitor.node_id,
            (NodeId{1, 1}));
  EXPECT_THAT(add_results,
              ElementsAre(Field(&MonitoredItemCreateResult::item_id, 7u)));
  EXPECT_THAT(subscription_state->remove_item_ids, ElementsAre(7u));
  EXPECT_THAT(remove_results, ElementsAre(Status{StatusCode::Good}));
}

TEST(MonitoredItemSubscriptionPump, DeliversNotificationsAndReadError) {
  TestExecutor executor;
  std::shared_ptr<FakeMonitoredItemSubscription::State> subscription_state =
      std::make_shared<FakeMonitoredItemSubscription::State>();
  subscription_state->read_results.emplace_back(
      std::vector<MonitoredItemNotification>{
          DataChangeNotification{.item_id = 7, .client_handle = 42}});
  subscription_state->read_results.emplace_back(
      Status{StatusCode::Bad_Disconnected});
  std::unique_ptr<FakeMonitoredItemSubscription> subscription =
      std::make_unique<FakeMonitoredItemSubscription>(subscription_state);
  FakeMonitoredItemService service{
      StatusOr<std::unique_ptr<MonitoredItemSubscription>>{
          std::move(subscription)}};
  std::vector<std::vector<MonitoredItemNotification>> notification_batches;
  std::vector<Status> errors;
  MonitoredItemSubscriptionPump pump{
      executor,
      service,
      {.max_batch_size = 5},
      [&](std::vector<MonitoredItemNotification> notifications) {
        notification_batches.emplace_back(std::move(notifications));
      },
      [&](Status status) { errors.emplace_back(std::move(status)); }};

  ASSERT_TRUE(pump.Start());
  Drain(executor);

  EXPECT_EQ(service.create_options.max_batch_size, 5u);
  EXPECT_THAT(subscription_state->read_next_max_counts, ElementsAre(5u, 5u));
  ASSERT_THAT(notification_batches, SizeIs(1));
  ASSERT_THAT(notification_batches[0], SizeIs(1));
  const auto* data_change =
      std::get_if<DataChangeNotification>(&notification_batches[0][0]);
  ASSERT_NE(data_change, nullptr);
  EXPECT_EQ(data_change->item_id, 7u);
  EXPECT_EQ(data_change->client_handle, 42u);
  EXPECT_THAT(errors, ElementsAre(Status{StatusCode::Bad_Disconnected}));
}

TEST(MonitoredItemSubscriptionPump, CloseSuppressesPendingReadError) {
  TestExecutor executor;
  std::shared_ptr<FakeMonitoredItemSubscription::State> subscription_state =
      std::make_shared<FakeMonitoredItemSubscription::State>();
  subscription_state->read_results.emplace_back(
      Status{StatusCode::Bad_Disconnected});
  std::unique_ptr<FakeMonitoredItemSubscription> subscription =
      std::make_unique<FakeMonitoredItemSubscription>(subscription_state);
  FakeMonitoredItemService service{
      StatusOr<std::unique_ptr<MonitoredItemSubscription>>{
          std::move(subscription)}};
  std::vector<Status> errors;
  MonitoredItemSubscriptionPump pump{
      executor,
      service,
      {},
      [](std::vector<MonitoredItemNotification>) {},
      [&](Status status) { errors.emplace_back(std::move(status)); }};

  ASSERT_TRUE(pump.Start());
  pump.Close(StatusCode::Bad_ServerWasShutDown);
  Drain(executor);

  EXPECT_EQ(subscription_state->close_status,
            StatusCode::Bad_ServerWasShutDown);
  EXPECT_THAT(errors, IsEmpty());
}

}  // namespace
}  // namespace scada
