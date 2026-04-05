#pragma once

#include "scada/client_monitored_item.h"
#include "scada/monitored_item_service_mock.h"
#include "scada/monitoring_parameters.h"

#include <gmock/gmock.h>

namespace scada {

struct ClientTestMonitoredItem {
  MockDataChangeHandler data_change_handler;
  MockEventHandler event_handler;

  scada::StatusCode status_code = StatusCode::Bad_Disconnected;
  scada::DataValue data_value;

  scada::monitored_item monitored_item;
};

class ClientTest : public testing::Test {
 public:
  std::unique_ptr<ClientTestMonitoredItem> SubscribeValue(
      const scada::node& node) const;
  std::unique_ptr<ClientTestMonitoredItem> SubscribeEvents(
      const scada::node& node,
      const scada::EventFilter& filter = {}) const;

  void ExpectValue(ClientTestMonitoredItem& monitored_value,
                   const Variant& value) const;

  promise<> ExpectValue(const scada::node& node, const Variant& value) const;
};

inline std::unique_ptr<ClientTestMonitoredItem> ClientTest::SubscribeValue(
    const scada::node& node) const {
  using namespace testing;

  auto monitored_value = std::make_unique<ClientTestMonitoredItem>();

  EXPECT_CALL(monitored_value->data_change_handler,
              Call(Field(&DataValue::status_code, StatusCode::Good)));

  monitored_value->monitored_item.subscribe_value(
      node, /*params=*/{},
      [&monitored_value =
           *monitored_value](const scada::DataValue& data_value) {
        monitored_value.status_code = data_value.status_code;
        monitored_value.data_value = data_value;
        monitored_value.data_change_handler.Call(data_value);
      });

  EXPECT_TRUE(monitored_value->monitored_item.subscribed());
  EXPECT_TRUE(scada::IsGood(monitored_value->status_code));

  return monitored_value;
}

inline std::unique_ptr<ClientTestMonitoredItem> ClientTest::SubscribeEvents(
    const scada::node& node,
    const scada::EventFilter& filter) const {
  using namespace testing;

  auto monitored_value = std::make_unique<ClientTestMonitoredItem>();

  EXPECT_CALL(monitored_value->event_handler,
              Call(Status{StatusCode::Good}, _));

  monitored_value->monitored_item.subscribe_events(
      node, {.filter = filter},
      [&monitored_value = *monitored_value](const scada::Status& status,
                                            const std::any& event) {
        monitored_value.status_code = status.code();
        monitored_value.event_handler.Call(status, event);
      });

  EXPECT_TRUE(monitored_value->monitored_item.subscribed());
  EXPECT_TRUE(scada::IsGood(monitored_value->status_code));

  return monitored_value;
}

inline void ClientTest::ExpectValue(ClientTestMonitoredItem& monitored_value,
                                    const Variant& value) const {
  using namespace testing;

  EXPECT_CALL(monitored_value.data_change_handler,
              Call(AllOf(Field(&DataValue::value, value),
                         Field(&DataValue::status_code, StatusCode::Good))));
}

inline promise<> ClientTest::ExpectValue(const scada::node& node,
                                         const Variant& value) const {
  using namespace testing;

  auto monitored_value = std::make_shared<ClientTestMonitoredItem>();

  promise<> promise;
  EXPECT_CALL(monitored_value->data_change_handler,
              Call(AllOf(Field(&DataValue::value, value),
                         Field(&DataValue::status_code, StatusCode::Good))))
      .WillOnce(Invoke([promise]() mutable { promise.resolve(); }));
  return promise;
}

}  // namespace scada
