#pragma once

#include "core/client.h"
#include "core/monitored_item_service_mock.h"

#include <gmock/gmock.h>

namespace scada {

struct TestMonitoredItem {
  MockDataChangeHandler data_change_handler;
  MockEventHandler event_handler;

  scada::StatusCode status_code = StatusCode::Bad_Disconnected;
  scada::DataValue data_value;

  scada::monitored_item monitored_item;
};

class ClientTest : public testing::Test {
 public:
  std::unique_ptr<TestMonitoredItem> SubscribeValue(
      const scada::node& node) const;
  std::unique_ptr<TestMonitoredItem> SubscribeEvents(
      const scada::node& node,
      const scada::MonitoringParameters& params = {}) const;

  void ExpectValue(TestMonitoredItem& monitored_value,
                   const Variant& value) const;
};

inline std::unique_ptr<TestMonitoredItem> ClientTest::SubscribeValue(
    const scada::node& node) const {
  using namespace testing;

  auto monitored_value = std::make_unique<TestMonitoredItem>();

  EXPECT_CALL(monitored_value->data_change_handler,
              Call(Field(&DataValue::status_code, StatusCode::Good)));

  monitored_value->monitored_item =
      node.subscribe_value([&monitored_value = *monitored_value](
                               const scada::DataValue& data_value) {
        monitored_value.status_code = data_value.status_code;
        monitored_value.data_value = data_value;
        monitored_value.data_change_handler.Call(data_value);
      });

  EXPECT_TRUE(monitored_value->monitored_item.subscribed());
  EXPECT_TRUE(scada::IsGood(monitored_value->status_code));

  return monitored_value;
}

inline std::unique_ptr<TestMonitoredItem> ClientTest::SubscribeEvents(
    const scada::node& node,
    const scada::MonitoringParameters& params) const {
  using namespace testing;

  auto monitored_value = std::make_unique<TestMonitoredItem>();

  EXPECT_CALL(monitored_value->event_handler,
              Call(Status{StatusCode::Good}, _));

  monitored_value->monitored_item = node.subscribe_events(
      params, [&monitored_value = *monitored_value](const scada::Status& status,
                                                    const std::any& event) {
        monitored_value.status_code = status.code();
        monitored_value.event_handler.Call(status, event);
      });

  EXPECT_TRUE(monitored_value->monitored_item.subscribed());
  EXPECT_TRUE(scada::IsGood(monitored_value->status_code));

  return monitored_value;
}

inline void ClientTest::ExpectValue(TestMonitoredItem& monitored_value,
                                    const Variant& value) const {
  using namespace testing;

  EXPECT_CALL(monitored_value.data_change_handler,
              Call(AllOf(Field(&DataValue::value, value),
                         Field(&DataValue::status_code, StatusCode::Good))));
}

}  // namespace scada
