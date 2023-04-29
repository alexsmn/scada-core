#pragma once

#include "core/client.h"

#include <gmock/gmock.h>

namespace scada {

struct TestMonitoredItem {
  const DataValue& data_value() const { return monitored_item.data_value(); }
  const Variant& value() const { return data_value().value; }

  testing::MockFunction<void(const DataValue& data_value)> data_change_handler;
  testing::MockFunction<void(const Status& status, const std::any& event)>
      event_handler;

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

  monitored_value->monitored_item = node.subscribe_value(
      monitored_value->data_change_handler.AsStdFunction());

  EXPECT_TRUE(monitored_value->monitored_item.subscribed());
  EXPECT_TRUE(IsGood(monitored_value->monitored_item.status_code()));

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
      params, monitored_value->event_handler.AsStdFunction());

  EXPECT_TRUE(monitored_value->monitored_item.subscribed());
  EXPECT_TRUE(IsGood(monitored_value->monitored_item.status_code()));

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
