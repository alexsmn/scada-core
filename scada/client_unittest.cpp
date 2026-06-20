#include "scada/client.h"

#include "base/test/awaitable_test.h"
#include "base/test/test_executor.h"
#include "scada/client_monitored_item.h"
#include "scada/monitored_item_service_mock.h"
#include "scada/monitoring_parameters.h"

#include <gmock/gmock.h>

#include <chrono>
#include <thread>

using namespace testing;

namespace scada {
namespace {

class ClientTest : public Test {
 protected:
  // Pumps the executor until `predicate` holds or a bounded number of spins
  // elapse. The subscription that backs single monitored items reads
  // notifications through an asio `steady_timer`; resuming a parked read after
  // a notification is pushed requires the timer service to fire, which a single
  // `Drain` does not guarantee. This mirrors `WaitResult`'s drain/yield spin.
  void DrainUntil(const std::function<bool()>& predicate) {
    for (int i = 0; i < 1000 && !predicate(); ++i) {
      Drain(executor_);
      std::this_thread::yield();
    }
    Drain(executor_);
  }

  TestExecutor executor_;

  MockMonitoredItemService monitored_item_service_;

  // The client `monitored_item` helper drives the subscription that backs the
  // single item through a `LegacyMonitoredItemAdapter`, so it needs an executor
  // and the value/status are delivered asynchronously after the executor is
  // pumped.
  client client_{services{.monitored_item_service = &monitored_item_service_,
                          .monitored_item_executor = executor_}};

  inline static const NodeId node_id{1, 1};
};

// When the service cannot create the underlying monitored item, the
// subscription's `AddItems` reports a bad status per item. The legacy adapter
// surfaces that failure to the data-change handler as a bad-status value (after
// the executor is pumped), so the subscriber is told the item could not be
// monitored.
TEST_F(ClientTest, MonitoredItemSubscriptionFail) {
  EXPECT_CALL(monitored_item_service_, CreateMonitoredItem(_, _))
      .WillOnce(Return(nullptr));

  StatusCode reported_status = StatusCode::Good;
  auto data_change_handler = [&](const DataValue& data_value) {
    reported_status = data_value.status_code;
  };

  scada::monitored_item monitored_item;
  monitored_item.subscribe_value(client_.node(node_id), /*params=*/{},
                                 data_change_handler);

  DrainUntil([&] { return reported_status == StatusCode::Bad_WrongNodeId; });
  EXPECT_EQ(reported_status, StatusCode::Bad_WrongNodeId);
}

// After a successful subscription, a bad-status data change pushed by the
// server item propagates to the client data-change handler.
TEST_F(ClientTest, MonitoredItemSubscriptionClose) {
  EXPECT_CALL(monitored_item_service_, CreateMonitoredItem(_, _));

  MonitoredItemHandler server_item_handler;
  EXPECT_CALL(*monitored_item_service_.default_monitored_item, Subscribe(_))
      .WillOnce(SaveArg<0>(&server_item_handler));

  bool received_bad_status = false;
  auto data_change_handler = [&](const DataValue& data_value) {
    if (data_value.status_code == StatusCode::Bad_Disconnected) {
      received_bad_status = true;
    }
  };

  scada::monitored_item monitored_item;
  monitored_item.subscribe_value(client_.node(node_id),
                                 /*params=*/{}, data_change_handler);

  // Pump until the item is added and the underlying monitored item's
  // `Subscribe` handler is captured.
  DrainUntil([&] {
    return std::holds_alternative<DataChangeHandler>(server_item_handler) &&
           std::get<DataChangeHandler>(server_item_handler);
  });

  ASSERT_TRUE(std::holds_alternative<DataChangeHandler>(server_item_handler))
      << "expected the subscription to subscribe a data-change handler";
  std::get<DataChangeHandler>(server_item_handler)(
      MakeReadError(StatusCode::Bad_Disconnected));

  // Pump until the pushed notification flows through the subscription and is
  // fanned out to the client data-change handler.
  DrainUntil([&] { return received_bad_status; });

  EXPECT_TRUE(received_bad_status);
}

}  // namespace
}  // namespace scada
