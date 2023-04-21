#pragma once

#include "core/test/client.h"

#include <gmock/gmock.h>

namespace scada {

struct TestMonitoredItem {
  const DataValue& data_value() const { return monitored_value.data_value(); }
  const Variant& value() const { return data_value().value; }

  testing::MockFunction<void(const DataValue& data_value)> value_change_handler;
  client::monitored_item monitored_value;
};

class ClientTest : public testing::Test {
 public:
  std::unique_ptr<TestMonitoredItem> SubscribeValue(
      const client::node& node) const;

  void ExpectValue(TestMonitoredItem& monitored_value,
                   const Variant& value) const;
};

inline std::unique_ptr<TestMonitoredItem> ClientTest::SubscribeValue(
    const client::node& node) const {
  using namespace testing;

  auto monitored_value = std::make_unique<TestMonitoredItem>();

  EXPECT_CALL(monitored_value->value_change_handler,
              Call(Field(&DataValue::status_code, StatusCode::Good)));

  monitored_value->monitored_value = node.subscribe_value(
      monitored_value->value_change_handler.AsStdFunction());

  EXPECT_TRUE(monitored_value->monitored_value.subscribed());

  return monitored_value;
}

inline void ClientTest::ExpectValue(TestMonitoredItem& monitored_value,
                                    const Variant& value) const {
  using namespace testing;

  EXPECT_CALL(monitored_value.value_change_handler,
              Call(AllOf(Field(&DataValue::value, value),
                         Field(&DataValue::status_code, StatusCode::Good))));
}

}  // namespace scada
