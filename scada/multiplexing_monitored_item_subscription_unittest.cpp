#include "scada/multiplexing_monitored_item_subscription.h"

#include "base/test/awaitable_test.h"
#include "base/test/test_executor.h"
#include "scada/monitored_item_service.h"
#include "scada/serving_gate.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <deque>
#include <memory>
#include <utility>
#include <vector>

namespace scada {
namespace {

using namespace testing;

// --- ServingGate (the shared block/allow gate) ------------------------------

TEST(ServingGate, AllowedByDefault) {
  ServingGate gate;
  EXPECT_TRUE(gate.allowed());
}

TEST(ServingGate, BlockDisallowsAndSweepsRegisteredSubscriptions) {
  ServingGate gate;
  std::vector<StatusCode> swept;
  auto registration =
      gate.Register([&](StatusCode status) { swept.push_back(status); });

  gate.SetBlocked(StatusCode::Bad_LicenseExpired);

  EXPECT_FALSE(gate.allowed());
  EXPECT_EQ(gate.blocked_status(), StatusCode::Bad_LicenseExpired);
  EXPECT_THAT(swept, ElementsAre(StatusCode::Bad_LicenseExpired));

  gate.SetAllowed();
  EXPECT_TRUE(gate.allowed());
}

TEST(ServingGate, DroppedRegistrationIsNotSwept) {
  ServingGate gate;
  int calls = 0;
  {
    auto registration = gate.Register([&](StatusCode) { ++calls; });
  }

  gate.SetBlocked(StatusCode::Bad_LicenseExpired);

  EXPECT_EQ(calls, 0);
}

// --- Multiplexing subscription honoring the gate ----------------------------

// A backend subscription seeded with fixed add/read results, mirroring the pump
// unit test's fake so this test can drive item creation without a real backend.
class FakeBackendSubscription final : public MonitoredItemSubscription {
 public:
  struct State {
    std::vector<MonitoredItemCreateResult> add_results;
    std::deque<StatusOr<std::vector<MonitoredItemNotification>>> read_results;
  };

  explicit FakeBackendSubscription(std::shared_ptr<State> state)
      : state_{std::move(state)} {}

  Awaitable<std::vector<MonitoredItemCreateResult>> AddItems(
      std::vector<MonitoredItemCreateRequest>) override {
    co_return state_->add_results;
  }

  Awaitable<std::vector<Status>> RemoveItems(
      std::span<const MonitoredItemId> item_ids) override {
    co_return std::vector<Status>(item_ids.size(), Status{StatusCode::Good});
  }

  Awaitable<StatusOr<std::vector<MonitoredItemNotification>>> ReadNext(
      std::size_t) override {
    if (state_->read_results.empty()) {
      // No further backend activity — the pump stops reading. The already-added
      // item stays registered in the multiplexing subscription.
      co_return Status{StatusCode::Bad_Disconnected};
    }
    StatusOr<std::vector<MonitoredItemNotification>> result =
        std::move(state_->read_results.front());
    state_->read_results.pop_front();
    co_return result;
  }

  void Close(Status) override {}

 private:
  std::shared_ptr<State> state_;
};

class FakeBackendService final : public MonitoredItemService {
 public:
  explicit FakeBackendService(
      std::shared_ptr<FakeBackendSubscription::State> state)
      : state_{std::move(state)} {}

  StatusOr<std::unique_ptr<MonitoredItemSubscription>> CreateSubscription(
      ServiceContext,
      MonitoredItemSubscriptionOptions) override {
    return std::unique_ptr<MonitoredItemSubscription>{
        std::make_unique<FakeBackendSubscription>(state_)};
  }

 private:
  std::shared_ptr<FakeBackendSubscription::State> state_;
};

// When the gate blocks after an item is already being monitored, the item is
// swept to the blocked status so a client sees the outage even if its backend
// value never changes again.
TEST(MultiplexingSubscriptionServingGate, SweepsActiveItemOnBlock) {
  TestExecutor executor;
  ServingGate gate;

  auto backend_state = std::make_shared<FakeBackendSubscription::State>();
  backend_state->add_results.push_back(
      {.item_id = 1, .client_handle = 0, .status = StatusCode::Good});
  FakeBackendService service{backend_state};

  StatusOr<std::unique_ptr<MonitoredItemSubscription>> subscription =
      MakeMultiplexingMonitoredItemSubscription(
          executor,
          [&](const ReadValueId& value_id, const MonitoringParameters&) {
            return std::vector<MonitoredItemRoute>{
                {.service = &service, .value_id = value_id}};
          },
          {}, &gate);
  ASSERT_TRUE(subscription.ok());

  std::vector<MonitoredItemCreateRequest> requests{
      {.item_to_monitor = {.node_id = NodeId{1, 1}}, .client_handle = 77}};
  std::vector<MonitoredItemCreateResult> add_results =
      WaitAwaitable(executor, (*subscription)->AddItems(std::move(requests)));
  Drain(executor);
  ASSERT_THAT(add_results, SizeIs(1));
  ASSERT_EQ(add_results[0].status, Status{StatusCode::Good});
  const MonitoredItemId router_item_id = add_results[0].item_id;

  // Runtime license expiry blocks the gate.
  gate.SetBlocked(StatusCode::Bad_LicenseExpired);

  StatusOr<std::vector<MonitoredItemNotification>> notifications =
      WaitAwaitable(executor, (*subscription)->ReadNext(10));
  ASSERT_TRUE(notifications.ok());
  ASSERT_THAT(*notifications, SizeIs(1));
  const auto* status_note =
      std::get_if<ItemStatusNotification>(&notifications->front());
  ASSERT_NE(status_note, nullptr);
  EXPECT_EQ(status_note->item_id, router_item_id);
  EXPECT_EQ(status_note->client_handle, 77u);
  EXPECT_EQ(status_note->status.code(), StatusCode::Bad_LicenseExpired);
}

// A backend sample delivered while the gate is blocked is rewritten to the
// blocked status, so a live value does not leak through during the outage.
TEST(MultiplexingSubscriptionServingGate, RewritesBackendSampleWhileBlocked) {
  TestExecutor executor;
  ServingGate gate;
  gate.SetBlocked(StatusCode::Bad_LicenseExpired);

  auto backend_state = std::make_shared<FakeBackendSubscription::State>();
  backend_state->add_results.push_back(
      {.item_id = 1, .client_handle = 0, .status = StatusCode::Good});
  // The backend delivers one good data-change; `client_handle` carries the
  // router item id the multiplexing layer assigned (the first item -> 1).
  backend_state->read_results.push_back(std::vector<MonitoredItemNotification>{
      DataChangeNotification{.item_id = 1,
                             .client_handle = 1,
                             .value = DataValue{Variant{42}, Qualifier{},
                                                DateTime{}, DateTime{}}}});
  FakeBackendService service{backend_state};

  StatusOr<std::unique_ptr<MonitoredItemSubscription>> subscription =
      MakeMultiplexingMonitoredItemSubscription(
          executor,
          [&](const ReadValueId& value_id, const MonitoringParameters&) {
            return std::vector<MonitoredItemRoute>{
                {.service = &service, .value_id = value_id}};
          },
          {}, &gate);
  ASSERT_TRUE(subscription.ok());

  std::vector<MonitoredItemCreateRequest> requests{
      {.item_to_monitor = {.node_id = NodeId{1, 1}}, .client_handle = 77}};
  WaitAwaitable(executor, (*subscription)->AddItems(std::move(requests)));
  Drain(executor);

  StatusOr<std::vector<MonitoredItemNotification>> notifications =
      WaitAwaitable(executor, (*subscription)->ReadNext(10));
  ASSERT_TRUE(notifications.ok());
  const DataChangeNotification* data_change = nullptr;
  for (const auto& notification : *notifications) {
    if (const auto* candidate =
            std::get_if<DataChangeNotification>(&notification)) {
      data_change = candidate;
    }
  }
  ASSERT_NE(data_change, nullptr);
  EXPECT_EQ(data_change->value.status_code, StatusCode::Bad_LicenseExpired);
}

}  // namespace
}  // namespace scada
