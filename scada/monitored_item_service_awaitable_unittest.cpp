#include "scada/monitored_item_service_awaitable.h"

#include "base/test/awaitable_test.h"
#include "base/test/test_executor.h"
#include "scada/co_result.h"
#include "scada/item_factory_subscription.h"
#include "scada/monitoring_parameters.h"
#include "scada/read_value_id.h"
#include "scada/status_or.h"
#include "scada/test/test_monitored_item.h"

#include <deque>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <map>

namespace scada {
namespace {

using namespace testing;

class TestMonitoredItemService : public MonitoredItemService {
 public:
  std::shared_ptr<MonitoredItem> CreateMonitoredItem(
      const ReadValueId& value_id,
      const MonitoringParameters& params) {
    (void)params;
    auto i = items.find(value_id.node_id);
    return i == items.end() ? nullptr : i->second;
  }

  StatusOr<std::unique_ptr<MonitoredItemSubscription>> CreateSubscription(
      ServiceContext /*context*/,
      MonitoredItemSubscriptionOptions options) override {
    return MakeItemFactorySubscription(
        [this](const ReadValueId& value_id,
               const MonitoringParameters& params) {
          return CreateMonitoredItem(value_id, params);
        },
        options);
  }

  std::map<NodeId, std::shared_ptr<TestMonitoredItem>> items;
};

class FakeMonitoredItemSubscription final : public MonitoredItemSubscription {
 public:
  struct State {
    std::vector<MonitoredItemCreateRequest> add_requests;
    std::vector<MonitoredItemCreateResult> add_results;
    std::deque<StatusOr<std::vector<MonitoredItemNotification>>> read_results;
    std::vector<std::size_t> read_next_max_counts;
    Status close_status = StatusCode::Good;
  };

  explicit FakeMonitoredItemSubscription(std::shared_ptr<State> state)
      : state_{std::move(state)} {}

  Awaitable<std::vector<MonitoredItemCreateResult>> AddItems(
      std::vector<MonitoredItemCreateRequest> requests) override {
    state_->add_requests = std::move(requests);
    co_return std::move(state_->add_results);
  }

  Awaitable<std::vector<Status>> RemoveItems(
      std::span<const MonitoredItemId> item_ids) override {
    std::vector<Status> results(item_ids.size(), StatusCode::Good);
    co_return results;
  }

  CoStatusOr<std::vector<MonitoredItemNotification>> ReadNext(
      std::size_t max_count) override {
    state_->read_next_max_counts.push_back(max_count);
    if (state_->read_results.empty()) {
      co_return Status{StatusCode::Bad_Disconnected};
    }

    auto result = std::move(state_->read_results.front());
    state_->read_results.pop_front();
    co_return result;
  }

  void Close(Status status) override {
    state_->close_status = std::move(status);
  }

 private:
  std::shared_ptr<State> state_;
};

class FakeSubscriptionService : public MonitoredItemService {
 public:
  explicit FakeSubscriptionService(
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

TEST(MonitoredItemServiceAwaitable, ReadsInitialValuesInInputOrder) {
  TestExecutor executor;
  TestMonitoredItemService service;
  auto first_item = std::make_shared<TestMonitoredItem>();
  auto second_item = std::make_shared<TestMonitoredItem>();
  service.items.emplace(NodeId{1, 1}, first_item);
  service.items.emplace(NodeId{2, 1}, second_item);

  const std::vector<ReadValueId> inputs{{NodeId{1, 1}}, {NodeId{2, 1}}};
  auto read_result =
      StartAwaitable(executor, ReadInitialValuesAsync(executor, service, inputs,
                                                      /*params=*/{}));

  Drain(executor);
  second_item->NotifyDataChange(DataValue{Variant{22}, {}, Time{}, Time{}});
  Drain(executor);
  first_item->NotifyDataChange(DataValue{Variant{11}, {}, Time{}, Time{}});

  EXPECT_THAT(WaitResult(executor, read_result),
              ElementsAre(Field(&DataValue::value, Variant{11}),
                          Field(&DataValue::value, Variant{22})));
}

// An item whose source never writes a value must not suspend the read forever:
// the initial-value wait is bounded and completes Bad_Timeout. This is what
// stalled fetches of device metrics the driver never produces.
TEST(MonitoredItemServiceAwaitable, ReadTimesOutWhenItemNeverReports) {
  TestExecutor executor;
  TestMonitoredItemService service;
  service.items.emplace(NodeId{1, 1}, std::make_shared<TestMonitoredItem>());

  auto read_result = StartAwaitable(
      executor,
      ReadInitialValueAsync(executor, service, ReadValueId{NodeId{1, 1}},
                            /*params=*/{},
                            /*timeout=*/std::chrono::milliseconds{20}));

  EXPECT_THAT(WaitResult(executor, read_result),
              Field(&DataValue::status_code, StatusCode::Bad_Timeout));
}

TEST(MonitoredItemServiceAwaitable, ReadInitialValueReturnsFirstDataChange) {
  TestExecutor executor;
  TestMonitoredItemService service;
  auto item = std::make_shared<TestMonitoredItem>();
  service.items.emplace(NodeId{1, 1}, item);

  auto read_result =
      StartAwaitable(executor, ReadInitialValueAsync(executor, service,
                                                     ReadValueId{NodeId{1, 1}},
                                                     /*params=*/{}));

  Drain(executor);
  EXPECT_FALSE(read_result->done);

  item->NotifyDataChange(DataValue{Variant{77}, {}, Time{}, Time{}});

  auto data_value = WaitResult(executor, read_result);
  EXPECT_EQ(data_value.status_code, StatusCode::Good);
  EXPECT_EQ(data_value.value, Variant{77});
}

TEST(MonitoredItemServiceAwaitable,
     ReadInitialValueReturnsReadErrorForMissingItem) {
  TestExecutor executor;
  TestMonitoredItemService service;

  auto read_result =
      StartAwaitable(executor, ReadInitialValueAsync(executor, service,
                                                     ReadValueId{NodeId{1, 1}},
                                                     /*params=*/{}));

  auto data_value = WaitResult(executor, read_result);
  EXPECT_EQ(data_value.status_code, StatusCode::Bad_WrongNodeId);
}

TEST(MonitoredItemServiceAwaitable,
     ReadInitialValueReturnsReadErrorForSubscriptionFailure) {
  TestExecutor executor;
  FakeSubscriptionService service{
      StatusOr<std::unique_ptr<MonitoredItemSubscription>>{
          StatusCode::Bad_Disconnected}};

  auto data_value =
      WaitAwaitable(executor, ReadInitialValueAsync(executor, service,
                                                    ReadValueId{NodeId{1, 1}},
                                                    /*params=*/{}));

  EXPECT_EQ(data_value.status_code, StatusCode::Bad_Disconnected);
}

TEST(MonitoredItemServiceAwaitable,
     ReadInitialValueReturnsItemStatusNotification) {
  TestExecutor executor;
  auto subscription_state =
      std::make_shared<FakeMonitoredItemSubscription::State>();
  auto subscription =
      std::make_unique<FakeMonitoredItemSubscription>(subscription_state);
  subscription_state->add_results.push_back(
      {.item_id = 1, .client_handle = 0, .status = StatusCode::Good});
  subscription_state->read_results.emplace_back(
      std::vector<MonitoredItemNotification>{
          ItemStatusNotification{.item_id = 1,
                                 .client_handle = 0,
                                 .status = StatusCode::Bad_WrongAttributeId}});
  FakeSubscriptionService service{
      StatusOr<std::unique_ptr<MonitoredItemSubscription>>{
          std::move(subscription)}};

  auto data_value = WaitAwaitable(
      executor,
      ReadInitialValueAsync(executor, service,
                            ReadValueId{NodeId{1, 1}, AttributeId::Value},
                            /*params=*/{}));

  EXPECT_EQ(data_value.status_code, StatusCode::Bad_WrongAttributeId);
  ASSERT_THAT(subscription_state->add_requests, SizeIs(1));
  EXPECT_EQ(subscription_state->add_requests[0].client_handle, 0u);
  EXPECT_EQ(subscription_state->add_requests[0].item_to_monitor.node_id,
            (NodeId{1, 1}));
  EXPECT_THAT(subscription_state->read_next_max_counts, ElementsAre(1u));
}

TEST(MonitoredItemSubscription, LegacyAdapterAddsAndReadsDataChanges) {
  TestExecutor executor;
  TestMonitoredItemService service;
  auto item = std::make_shared<TestMonitoredItem>();
  service.items.emplace(NodeId{1, 1}, item);

  auto subscription =
      service.CreateSubscription(ServiceContext{}, {.max_batch_size = 16});
  ASSERT_TRUE(subscription.ok()) << subscription.status();

  std::vector<MonitoredItemCreateRequest> requests;
  requests.push_back(
      {.item_to_monitor = ReadValueId{NodeId{1, 1}, AttributeId::Value},
       .client_handle = 42});
  auto add_results =
      WaitAwaitable(executor, (*subscription)->AddItems(std::move(requests)));
  ASSERT_THAT(add_results, SizeIs(1));
  EXPECT_EQ(add_results[0].status.code(), StatusCode::Good);
  EXPECT_EQ(add_results[0].item_id, 1u);
  EXPECT_EQ(add_results[0].client_handle, 42u);

  item->NotifyDataChange(DataValue{Variant{11}, {}, Time{}, Time{}});

  auto notifications = WaitAwaitable(executor, (*subscription)->ReadNext(8));
  ASSERT_TRUE(notifications.ok()) << notifications.status();
  ASSERT_THAT(*notifications, SizeIs(1));

  const auto* data_change =
      std::get_if<DataChangeNotification>(&notifications->front());
  ASSERT_NE(data_change, nullptr);
  EXPECT_EQ(data_change->item_id, 1u);
  EXPECT_EQ(data_change->client_handle, 42u);
  EXPECT_EQ(data_change->value.value, Variant{11});
}

TEST(MonitoredItemSubscription, LegacyAdapterReadNextWaitsForNotification) {
  TestExecutor executor;
  TestMonitoredItemService service;
  auto item = std::make_shared<TestMonitoredItem>();
  service.items.emplace(NodeId{1, 1}, item);

  auto subscription = service.CreateSubscription(ServiceContext{}, {});
  ASSERT_TRUE(subscription.ok()) << subscription.status();

  std::vector<MonitoredItemCreateRequest> requests;
  requests.push_back(
      {.item_to_monitor = ReadValueId{NodeId{1, 1}, AttributeId::Value}});
  auto add_results =
      WaitAwaitable(executor, (*subscription)->AddItems(std::move(requests)));
  ASSERT_THAT(add_results, SizeIs(1));

  auto read_result =
      StartAwaitable(executor, (*subscription)->ReadNext(/*max_count=*/8));
  Drain(executor);
  EXPECT_FALSE(read_result->done);

  item->NotifyDataChange(DataValue{Variant{12}, {}, Time{}, Time{}});

  auto notifications = WaitResult(executor, read_result);
  ASSERT_TRUE(notifications.ok()) << notifications.status();
  ASSERT_THAT(*notifications, SizeIs(1));
  const auto* data_change =
      std::get_if<DataChangeNotification>(&notifications->front());
  ASSERT_NE(data_change, nullptr);
  EXPECT_EQ(data_change->value.value, Variant{12});
}

TEST(MonitoredItemSubscription, LegacyAdapterReportsMissingItemsPerRequest) {
  TestExecutor executor;
  TestMonitoredItemService service;

  auto subscription = service.CreateSubscription(ServiceContext{}, {});
  ASSERT_TRUE(subscription.ok()) << subscription.status();

  std::vector<MonitoredItemCreateRequest> requests;
  requests.push_back(
      {.item_to_monitor = ReadValueId{NodeId{1, 1}, AttributeId::Value},
       .client_handle = 7});
  auto add_results =
      WaitAwaitable(executor, (*subscription)->AddItems(std::move(requests)));

  ASSERT_THAT(add_results, SizeIs(1));
  EXPECT_EQ(add_results[0].item_id, 0u);
  EXPECT_EQ(add_results[0].client_handle, 7u);
  EXPECT_EQ(add_results[0].status.code(), StatusCode::Bad_WrongNodeId);
}

TEST(MonitoredItemSubscription, LegacyAdapterReportsBadAttributePerRequest) {
  TestExecutor executor;
  TestMonitoredItemService service;

  auto subscription = service.CreateSubscription(ServiceContext{}, {});
  ASSERT_TRUE(subscription.ok()) << subscription.status();

  std::vector<MonitoredItemCreateRequest> requests;
  requests.push_back(
      {.item_to_monitor = ReadValueId{NodeId{1, 1}, AttributeId::BrowseName},
       .client_handle = 8});
  auto add_results =
      WaitAwaitable(executor, (*subscription)->AddItems(std::move(requests)));

  ASSERT_THAT(add_results, SizeIs(1));
  EXPECT_EQ(add_results[0].item_id, 0u);
  EXPECT_EQ(add_results[0].client_handle, 8u);
  EXPECT_EQ(add_results[0].status.code(), StatusCode::Bad_WrongAttributeId);
}

TEST(MonitoredItemSubscription, LegacyAdapterRemovesItems) {
  TestExecutor executor;
  TestMonitoredItemService service;
  service.items.emplace(NodeId{1, 1}, std::make_shared<TestMonitoredItem>());

  auto subscription = service.CreateSubscription(ServiceContext{}, {});
  ASSERT_TRUE(subscription.ok()) << subscription.status();

  std::vector<MonitoredItemCreateRequest> requests;
  requests.push_back(
      {.item_to_monitor = ReadValueId{NodeId{1, 1}, AttributeId::Value}});
  auto add_results =
      WaitAwaitable(executor, (*subscription)->AddItems(std::move(requests)));
  ASSERT_THAT(add_results, SizeIs(1));

  const MonitoredItemId item_id = add_results[0].item_id;
  auto remove_results = WaitAwaitable(
      executor, (*subscription)->RemoveItems(std::span{&item_id, 1}));
  ASSERT_THAT(remove_results, SizeIs(1));
  EXPECT_EQ(remove_results[0].code(), StatusCode::Good);

  auto remove_again = WaitAwaitable(
      executor, (*subscription)->RemoveItems(std::span{&item_id, 1}));
  ASSERT_THAT(remove_again, SizeIs(1));
  EXPECT_EQ(remove_again[0].code(), StatusCode::Bad_MonitoredItemIdInvalid);
}

}  // namespace
}  // namespace scada
