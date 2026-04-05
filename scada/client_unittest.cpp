#include "scada/client.h"

#include "scada/client_monitored_item.h"
#include "scada/monitored_item_service_mock.h"
#include "scada/monitoring_parameters.h"

#include <gmock/gmock.h>

using namespace testing;

namespace scada {

class ClientTest : public Test {
 protected:
  MockMonitoredItemService monitored_item_service_;

  client client_{services{.monitored_item_service = &monitored_item_service_}};

  inline static const NodeId node_id{1, 1};
};

TEST_F(ClientTest, MonitoredItemSubscriptionFail) {
  EXPECT_CALL(monitored_item_service_, CreateMonitoredItem(_, _))
      .WillOnce(Return(nullptr));

  MockDataChangeHandler data_change_handler;

  EXPECT_CALL(data_change_handler, Call(Field(&DataValue::status_code,
                                              StatusCode::Bad_WrongNodeId)));

  scada::monitored_item monitored_item;
  monitored_item.subscribe_value(client_.node(node_id), /*params=*/{},
                                 data_change_handler.AsStdFunction());
}

TEST_F(ClientTest, MonitoredItemSubscriptionClose) {
  EXPECT_CALL(monitored_item_service_, CreateMonitoredItem(_, _));

  MonitoredItemHandler server_item_handler;
  EXPECT_CALL(*monitored_item_service_.default_monitored_item, Subscribe(_))
      .WillOnce(SaveArg<0>(&server_item_handler));

  MockDataChangeHandler data_change_handler;
  EXPECT_CALL(data_change_handler, Call(Field(&DataValue::status_code,
                                              StatusCode::Bad_Disconnected)));

  scada::monitored_item monitored_item;
  monitored_item.subscribe_value(client_.node(node_id),
                                 /*params=*/{},
                                 data_change_handler.AsStdFunction());

  std::get<DataChangeHandler>(std::move(server_item_handler))(
      MakeReadError(StatusCode::Bad_Disconnected));
}

}  // namespace scada