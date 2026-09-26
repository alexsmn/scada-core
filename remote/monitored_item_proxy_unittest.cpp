#include "remote/monitored_item_proxy.h"

#include "remote/message_sender_fake.h"
#include "remote/monitored_item_router_fake.h"
#include "remote/protocol_utils.h"

#include <string>
#include <vector>

#include <gmock/gmock.h>

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

  // The proxy only ever issues requests; it never sends a bare message. This
  // is the strictness the StrictMock this fixture used to hold enforced.
  void TearDown() override {
    EXPECT_THAT(message_sender_.sent_messages(), IsEmpty());
  }

  // The kind of every request sent so far, oldest first: "create", "delete",
  // or "other" for anything else.
  std::vector<std::string> SentRequestKinds() const;

  // Plays the server: completes the most recent request with `response`.
  void RespondToLastRequest(const protocol::Response& response);

  StrictMock<DataChangeHandler> data_change_handler_;
  MonitoredItemRouterFake monitored_item_router_;
  MessageSenderFake message_sender_;

  std::shared_ptr<MonitoredItemProxy> monitored_item_;

  inline static const scada::NodeId kNodeId{12, 34};
  // `inline`, like every other constant here: the state assertions below bind
  // these by reference, which odr-uses them and needs a definition. Without it
  // the Release build folds the value and links, and only Debug fails.
  inline static const int kSubscriptionId = 567;
  inline static const MonitoredItemId kMonitoredItemId = 11122;
  inline static const scada::Time kTimeStamp = scada::Now();
  inline static const scada::DataValue kDataValue{123,
                                                  {},
                                                  kTimeStamp,
                                                  kTimeStamp};
  inline static const scada::DataValue kDataValueFailed{scada::StatusCode::Bad,
                                                        kTimeStamp};
};

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

std::vector<std::string> MonitoredItemProxyTest::SentRequestKinds() const {
  std::vector<std::string> kinds;
  for (const auto& sent : message_sender_.requests()) {
    if (sent.request.has_create_monitored_item())
      kinds.push_back("create");
    else if (sent.request.has_delete_monitored_item())
      kinds.push_back("delete");
    else
      kinds.push_back("other");
  }
  return kinds;
}

void MonitoredItemProxyTest::RespondToLastRequest(
    const protocol::Response& response) {
  ASSERT_FALSE(message_sender_.requests().empty());
  // Copied out first: the handler may send, and a send grows the vector the
  // reference would point into.
  MessageSender::ResponseHandler handler =
      message_sender_.requests().back().response_handler;
  ASSERT_TRUE(handler);
  handler(response);
}

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

  monitored_item_->Subscribe(data_change_handler_.handler);

  ASSERT_THAT(SentRequestKinds(), ElementsAre("create"));
}

void MonitoredItemProxyTest::
    CreateMonitoredItem_OpenChannel_Subscribe_CreateStubSuccessful() {
  CreateMonitoredItem_OpenChannel_Subscribe();

  // Create stub successful

  RespondToLastRequest(MakeCreateMonitoredItemResponse(scada::StatusCode::Good,
                                                       kMonitoredItemId));

  EXPECT_THAT(monitored_item_router_.registered_ids(),
              ElementsAre(kMonitoredItemId));
  EXPECT_EQ(monitored_item_router_.observer(kMonitoredItemId),
            monitored_item_.get());
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

  EXPECT_CALL(data_change_handler_, OnDataChange(IsOnline(false)));

  monitored_item_->OnChannelClosed();

  EXPECT_THAT(monitored_item_router_.registered_ids(), IsEmpty());
}

void MonitoredItemProxyTest::
    CreateMonitoredItem_OpenChannel_Subscribe_CreateStubSuccessful_DataChangeFailed_CloseChannel() {
  CreateMonitoredItem_OpenChannel_Subscribe_CreateStubSuccessful();

  // Data change failed

  EXPECT_CALL(data_change_handler_, OnDataChange(kDataValueFailed));

  monitored_item_->OnDataChange(kDataValueFailed);

  EXPECT_THAT(monitored_item_router_.registered_ids(), IsEmpty());

  // Close channel

  monitored_item_->OnChannelClosed();
}

TEST_F(MonitoredItemProxyTest,
       CreateMonitoredItem_OpenChannel_Subscribe_DeleteMonitoredItem) {
  CreateMonitoredItem_OpenChannel_Subscribe();

  // Delete monitored item

  // TODO: Expect unsubscription.

  monitored_item_.reset();

  // Today a pending create is abandoned without a delete; the StrictMock this
  // fixture used to hold enforced that implicitly, so it is stated here.
  EXPECT_THAT(SentRequestKinds(), ElementsAre("create"));
}
TEST_F(
    MonitoredItemProxyTest,
    CreateMonitoredItem_OpenChannel_Subscribe_CreateStubSuccessful_DataChange_DeleteMonitoredItem) {
  CreateMonitoredItem_OpenChannel_Subscribe_CreateStubSuccessful_DataChange();

  // Delete monitored item

  monitored_item_.reset();

  EXPECT_THAT(SentRequestKinds(), ElementsAre("create", "delete"));
  EXPECT_THAT(monitored_item_router_.registered_ids(), IsEmpty());
}

TEST_F(
    MonitoredItemProxyTest,
    CreateMonitoredItem_OpenChannel_Subscribe_CreateStubSuccessful_DataChange_CloseChannel_DeleteMonitoredItem) {
  CreateMonitoredItem_OpenChannel_Subscribe_CreateStubSuccessful_DataChange_CloseChannel();

  // Delete monitored item

  monitored_item_.reset();

  // The channel is gone, so there is nobody to send a delete to.
  EXPECT_THAT(SentRequestKinds(), ElementsAre("create"));
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

  RespondToLastRequest(MakeCreateMonitoredItemResponse(kErrorCode, 0));

  // Delete monitored item

  monitored_item_.reset();

  // A create the server refused left nothing to delete.
  EXPECT_THAT(SentRequestKinds(), ElementsAre("create"));
}

TEST_F(
    MonitoredItemProxyTest,
    CreateMonitoredItem_Subscribe_OpenChannel_CreateStubSuccessful_DeleteMonitoredItem) {
  CreateMonitoredItem();

  // Subscribe

  monitored_item_->Subscribe(data_change_handler_.handler);

  // Open channel

  EXPECT_CALL(data_change_handler_, OnDataChange(_));

  monitored_item_->OnChannelOpened(monitored_item_router_, message_sender_,
                                   kSubscriptionId);

  ASSERT_THAT(SentRequestKinds(), ElementsAre("create"));

  // Create stub successful

  RespondToLastRequest(MakeCreateMonitoredItemResponse(scada::StatusCode::Good,
                                                       kMonitoredItemId));

  EXPECT_THAT(monitored_item_router_.registered_ids(),
              ElementsAre(kMonitoredItemId));
  EXPECT_EQ(monitored_item_router_.observer(kMonitoredItemId),
            monitored_item_.get());

  // Delete monitored item

  monitored_item_.reset();

  EXPECT_THAT(SentRequestKinds(), ElementsAre("create", "delete"));
  EXPECT_THAT(monitored_item_router_.registered_ids(), IsEmpty());
}

TEST_F(
    MonitoredItemProxyTest,
    CreateMonitoredItem_OpenChannel_Subscribe_CreateStubSuccessful_DataChange_CloseChannel_OpenChannel_CreateStubSuccessful_DataChange_DeleteMonitoredItem) {
  CreateMonitoredItem_OpenChannel_Subscribe_CreateStubSuccessful_DataChange_CloseChannel();

  // Open channel

  EXPECT_CALL(data_change_handler_, OnDataChange(IsOnline(false)));

  monitored_item_->OnChannelOpened(monitored_item_router_, message_sender_,
                                   kSubscriptionId);

  ASSERT_THAT(SentRequestKinds(), ElementsAre("create", "create"));

  // Create stub successful

  RespondToLastRequest(MakeCreateMonitoredItemResponse(scada::StatusCode::Good,
                                                       kMonitoredItemId));

  // Re-registered under the same id after the channel came back -- the
  // reopen path must not leave the router holding a stale entry, which the
  // fake's duplicate-registration Check would have caught.
  EXPECT_THAT(monitored_item_router_.registered_ids(),
              ElementsAre(kMonitoredItemId));
  EXPECT_EQ(monitored_item_router_.observer(kMonitoredItemId),
            monitored_item_.get());

  // Data change

  const scada::Time kTimeStamp3 = scada::Now();
  const scada::DataValue kDataValue3{321, {}, kTimeStamp3, kTimeStamp3};
  EXPECT_CALL(data_change_handler_, OnDataChange(kDataValue3));

  monitored_item_->OnDataChange(kDataValue3);

  // Delete monitored item

  monitored_item_.reset();

  EXPECT_THAT(SentRequestKinds(), ElementsAre("create", "create", "delete"));
  EXPECT_THAT(monitored_item_router_.registered_ids(), IsEmpty());
}
