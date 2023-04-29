#include "core/client.h"

#include "core/monitored_item_service_mock.h"

#include <gmock/gmock.h>

using namespace testing;

namespace scada {

class ClientTest : public Test {
 protected:
  MockMonitoredItemService monitored_item_service;

  client client{services{.monitored_item_service = &monitored_item_service}};

  inline static const NodeId node_id{1, 1};
};

TEST_F(ClientTest, MonitoredItemSubscriptionFail) {
  EXPECT_CALL(monitored_item_service, CreateMonitoredItem(_, _))
      .WillOnce(Return(nullptr));

  MockDataChangeHandler data_change_handler;

  EXPECT_CALL(data_change_handler, Call(Field(&DataValue::status_code,
                                              StatusCode::Bad_WrongNodeId)));

  auto monitored_item =
      client.node(node_id).subscribe_value(data_change_handler.AsStdFunction());
}

TEST_F(ClientTest, MonitoredItemSubscriptionClose) {
  EXPECT_CALL(monitored_item_service, CreateMonitoredItem(_, _));

  MonitoredItemHandler server_item_handler;
  EXPECT_CALL(*monitored_item_service.default_monitored_item, Subscribe(_))
      .WillOnce(SaveArg<0>(&server_item_handler));

  MockDataChangeHandler data_change_handler;
  EXPECT_CALL(data_change_handler, Call(Field(&DataValue::status_code,
                                              StatusCode::Bad_Disconnected)));

  auto monitored_item =
      client.node(node_id).subscribe_value(data_change_handler.AsStdFunction());

  std::get<DataChangeHandler>(std::move(server_item_handler))(
      MakeReadError(StatusCode::Bad_Disconnected));
}

}  // namespace scada