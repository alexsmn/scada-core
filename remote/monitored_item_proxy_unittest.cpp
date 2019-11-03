#include "remote/monitored_item_proxy.h"

#include "remote/message_sender_mock.h"
#include "remote/monitored_item_router_mock.h"
#include "remote/protocol_utils.h"

using namespace testing;

struct DataChangeHandler {
  MOCK_METHOD1(OnDataChange, void(const scada::DataValue& data_value));

  const scada::DataChangeHandler handler =
      [this](const scada::DataValue& data_value) { OnDataChange(data_value); };
};

class MonitoredItemProxyTest : public Test {
 protected:
  void CreateMonitoredItem();
  void CreateMonitoredItem_OpenChannel_Subscribe();
  void CreateMonitoredItem_OpenChannel_Subscribe_CreateStubSuccessful();
  void
  CreateMonitoredItem_OpenChannel_Subscribe_CreateStubSuccessful_DataChange();
  void
  CreateMonitoredItem_OpenChannel_Subscribe_CreateStubSuccessful_DataChange_CloseChannel();
  void
  CreateMonitoredItem_OpenChannel_Subscribe_CreateStubSuccessful_DataChangeFailed_CloseChannel();

  StrictMock<DataChangeHandler> data_change_handler_;
  StrictMock<MonitoredItemRouterMock> monitored_item_router_;
  StrictMock<MessageSenderMock> message_sender_;

  std::shared_ptr<MonitoredItemProxy> monitored_item_;

  MessageSender::ResponseHandler response_handler_;

  inline static const scada::NodeId kNodeId{12, 34};
  static const int kSubscriptionId = 567;
  static const MonitoredItemId kMonitoredItemId = 11122;
  inline static const scada::DateTime kTimeStamp = scada::DateTime::Now();
  inline static const scada::DataValue kDataValue{123,
                                                  {},
                                                  kTimeStamp,
                                                  kTimeStamp};
  inline static const scada::DataValue kDataValueFailed{scada::StatusCode::Bad,
                                                        kTimeStamp};
};

MATCHER(IsCreateMonitoredItemRequest, "IsCreateMonitoredItemRequest") {
  return arg.has_create_monitored_item();
}

MATCHER(IsDeleteMonitoredItemRequest, "IsDeleteMonitoredItemRequest") {
  return arg.has_delete_monitored_item();
}

MATCHER_P(IsOnline, online, "IsOnline") {
  return arg.qualifier.online() == online;
}

namespace {

protocol::Response MakeCreateMonitoredItemResponse(
    const scada::Status& status,
    MonitoredItemId monitored_item_id) {
  protocol::Response response;
  Convert(status, *response.mutable_status());
  response.mutable_create_monitored_item_result()->set_monitored_item_id(
      monitored_item_id);
  return response;
}

}  // namespace

// Create monitored item
//   Subscribe
//     Open channel
//   Open channel
//     Subscribe
//       Delete monitored item
//       Create stub failed ?
//       Create stub successful
//         Data change
//           Delete monitored item
//           Close channel
//             Delete monitored item
//             Open channel
//               Create stub failed ?
//               Create stub successful
//                 Data change
//                   Delete monitored item

void MonitoredItemProxyTest::CreateMonitoredItem() {
  monitored_item_ = std::make_shared<MonitoredItemProxy>(
      scada::ReadValueId{kNodeId, scada::AttributeId::Value},
      scada::MonitoringParameters{});
}

void MonitoredItemProxyTest::CreateMonitoredItem_OpenChannel_Subscribe() {
  CreateMonitoredItem();

  // Open channel

  monitored_item_->OnChannelOpened(monitored_item_router_, message_sender_,
                                   kSubscriptionId);

  // Subscribe

  EXPECT_CALL(message_sender_, Request(IsCreateMonitoredItemRequest(), _))
      .WillOnce(SaveArg<1>(&response_handler_));

  monitored_item_->SubscribeData(data_change_handler_.handler);
}

void MonitoredItemProxyTest::
    CreateMonitoredItem_OpenChannel_Subscribe_CreateStubSuccessful() {
  CreateMonitoredItem_OpenChannel_Subscribe();

  // Create stub successful

  EXPECT_CALL(
      monitored_item_router_,
      AddMonitoredItemDataObserver(kMonitoredItemId, Ref(*monitored_item_)));

  ASSERT_TRUE(response_handler_);
  response_handler_(MakeCreateMonitoredItemResponse(scada::StatusCode::Good,
                                                    kMonitoredItemId));
}

void MonitoredItemProxyTest::
    CreateMonitoredItem_OpenChannel_Subscribe_CreateStubSuccessful_DataChange() {
  CreateMonitoredItem_OpenChannel_Subscribe_CreateStubSuccessful();

  // Data change

  EXPECT_CALL(data_change_handler_, OnDataChange(kDataValue));

  monitored_item_->OnDataChange(kDataValue);
}

void MonitoredItemProxyTest::
    CreateMonitoredItem_OpenChannel_Subscribe_CreateStubSuccessful_DataChange_CloseChannel() {
  CreateMonitoredItem_OpenChannel_Subscribe_CreateStubSuccessful_DataChange();

  // Close channel

  EXPECT_CALL(monitored_item_router_,
              RemoveMonitoredItemDataObserver(kMonitoredItemId));

  EXPECT_CALL(data_change_handler_, OnDataChange(IsOnline(false)));

  monitored_item_->OnChannelClosed();
}

void MonitoredItemProxyTest::
    CreateMonitoredItem_OpenChannel_Subscribe_CreateStubSuccessful_DataChangeFailed_CloseChannel() {
  CreateMonitoredItem_OpenChannel_Subscribe_CreateStubSuccessful();

  // Data change failed

  EXPECT_CALL(monitored_item_router_,
              RemoveMonitoredItemDataObserver(kMonitoredItemId));
  EXPECT_CALL(data_change_handler_, OnDataChange(kDataValueFailed));

  monitored_item_->OnDataChange(kDataValueFailed);

  // Close channel

  monitored_item_->OnChannelClosed();
}

TEST_F(MonitoredItemProxyTest,
       CreateMonitoredItem_OpenChannel_Subscribe_DeleteMonitoredItem) {
  CreateMonitoredItem_OpenChannel_Subscribe();

  // Delete monitored item

  // TODO: Expect unsubscription.

  monitored_item_.reset();
}
TEST_F(
    MonitoredItemProxyTest,
    CreateMonitoredItem_OpenChannel_Subscribe_CreateStubSuccessful_DataChange_DeleteMonitoredItem) {
  CreateMonitoredItem_OpenChannel_Subscribe_CreateStubSuccessful_DataChange();

  // Delete monitored item

  EXPECT_CALL(monitored_item_router_,
              RemoveMonitoredItemDataObserver(kMonitoredItemId));
  EXPECT_CALL(message_sender_, Request(IsDeleteMonitoredItemRequest(), _));

  monitored_item_.reset();
}

TEST_F(
    MonitoredItemProxyTest,
    CreateMonitoredItem_OpenChannel_Subscribe_CreateStubSuccessful_DataChange_CloseChannel_DeleteMonitoredItem) {
  CreateMonitoredItem_OpenChannel_Subscribe_CreateStubSuccessful_DataChange_CloseChannel();

  // Delete monitored item

  monitored_item_.reset();
}

TEST_F(
    MonitoredItemProxyTest,
    CreateMonitoredItem_OpenChannel_Subscribe_CreateStubFailed_DeleteMonitoredItem) {
  CreateMonitoredItem_OpenChannel_Subscribe();

  // Create stub failed

  const auto kErrorCode = scada::StatusCode::Bad_WrongNodeId;

  EXPECT_CALL(
      data_change_handler_,
      OnDataChange(AllOf(Field(&scada::DataValue::status_code, kErrorCode),
                         Field(&scada::DataValue::qualifier,
                               Property(&scada::Qualifier::failed, true)))));

  response_handler_(MakeCreateMonitoredItemResponse(kErrorCode, 0));

  // Delete monitored item

  monitored_item_.reset();
}

TEST_F(
    MonitoredItemProxyTest,
    CreateMonitoredItem_Subscribe_OpenChannel_CreateStubSuccessful_DeleteMonitoredItem) {
  CreateMonitoredItem();

  // Subscribe

  monitored_item_->SubscribeData(data_change_handler_.handler);

  // Open channel

  EXPECT_CALL(data_change_handler_, OnDataChange(_));

  MessageSender::ResponseHandler response_handler;
  EXPECT_CALL(message_sender_, Request(IsCreateMonitoredItemRequest(), _))
      .WillOnce(SaveArg<1>(&response_handler));

  monitored_item_->OnChannelOpened(monitored_item_router_, message_sender_,
                                   kSubscriptionId);

  // Create stub successful

  EXPECT_CALL(
      monitored_item_router_,
      AddMonitoredItemDataObserver(kMonitoredItemId, Ref(*monitored_item_)));

  ASSERT_TRUE(response_handler);
  response_handler(MakeCreateMonitoredItemResponse(scada::StatusCode::Good,
                                                   kMonitoredItemId));

  // Delete monitored item

  EXPECT_CALL(monitored_item_router_,
              RemoveMonitoredItemDataObserver(kMonitoredItemId));
  EXPECT_CALL(message_sender_, Request(IsDeleteMonitoredItemRequest(), _));

  monitored_item_.reset();
}

TEST_F(
    MonitoredItemProxyTest,
    CreateMonitoredItem_OpenChannel_Subscribe_CreateStubSuccessful_DataChange_CloseChannel_OpenChannel_CreateStubSuccessful_DataChange_DeleteMonitoredItem) {
  CreateMonitoredItem_OpenChannel_Subscribe_CreateStubSuccessful_DataChange_CloseChannel();

  // Open channel

  MessageSender::ResponseHandler response_handler;
  EXPECT_CALL(message_sender_, Request(IsCreateMonitoredItemRequest(), _))
      .WillOnce(SaveArg<1>(&response_handler));

  EXPECT_CALL(data_change_handler_, OnDataChange(IsOnline(false)));

  monitored_item_->OnChannelOpened(monitored_item_router_, message_sender_,
                                   kSubscriptionId);

  // Create stub successful

  EXPECT_CALL(
      monitored_item_router_,
      AddMonitoredItemDataObserver(kMonitoredItemId, Ref(*monitored_item_)));

  ASSERT_TRUE(response_handler);
  response_handler(MakeCreateMonitoredItemResponse(scada::StatusCode::Good,
                                                   kMonitoredItemId));

  // Data change

  const scada::DateTime kTimeStamp3 = scada::DateTime::Now();
  const scada::DataValue kDataValue3{321, {}, kTimeStamp3, kTimeStamp3};
  EXPECT_CALL(data_change_handler_, OnDataChange(kDataValue3));

  monitored_item_->OnDataChange(kDataValue3);

  // Delete monitored item

  EXPECT_CALL(monitored_item_router_,
              RemoveMonitoredItemDataObserver(kMonitoredItemId));
  EXPECT_CALL(message_sender_, Request(IsDeleteMonitoredItemRequest(), _));

  monitored_item_.reset();
}
