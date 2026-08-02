#include "remote/node_management_stub.h"

#include "base/test/test_executor.h"
#include "remote/message_sender_mock.h"
#include "remote/protocol.h"
#include "remote/protocol_utils.h"
#include "scada/co_result.h"

#include <gmock/gmock.h>

using namespace testing;

namespace {

class TestNodeManagementService final : public scada::NodeManagementService {
 public:
  scada::CoStatusOr<std::vector<scada::AddNodesResult>> AddNodes(
      scada::ServiceContext /*context*/,
      std::vector<scada::AddNodesItem> inputs) override {
    add_nodes_called = true;
    last_add_nodes_inputs = std::move(inputs);
    co_return std::vector<scada::AddNodesResult>{
        {.status_code = scada::StatusCode::Good, .added_node_id = {2, 3}}};
  }

  scada::CoStatusOr<std::vector<scada::StatusCode>> DeleteNodes(
      scada::ServiceContext /*context*/,
      std::vector<scada::DeleteNodesItem> inputs) override {
    delete_nodes_called = true;
    last_delete_nodes_inputs = std::move(inputs);
    co_return std::vector<scada::StatusCode>{};
  }

  scada::CoStatusOr<std::vector<scada::StatusCode>> AddReferences(
      scada::ServiceContext /*context*/,
      std::vector<scada::AddReferencesItem>) override {
    co_return std::vector<scada::StatusCode>{};
  }

  scada::CoStatusOr<std::vector<scada::StatusCode>> DeleteReferences(
      scada::ServiceContext /*context*/,
      std::vector<scada::DeleteReferencesItem>) override {
    co_return std::vector<scada::StatusCode>{};
  }

  bool add_nodes_called = false;
  bool delete_nodes_called = false;
  std::vector<scada::AddNodesItem> last_add_nodes_inputs;
  std::vector<scada::DeleteNodesItem> last_delete_nodes_inputs;
};

TEST(NodeManagementStubTest, AddNodesUsesCoroutineServiceBoundAtSessionEdge) {
  TestExecutor executor;
  auto sender = std::make_shared<StrictMock<MessageSenderMock>>();
  TestNodeManagementService service;

  auto stub = std::make_shared<NodeManagementStub>(
      executor, sender, service, scada::ServiceContext{}.with_user_id({1, 1}));

  EXPECT_CALL(*sender, Send(_)).WillOnce(Invoke([](protocol::Message& message) {
    ASSERT_EQ(message.responses_size(), 1);
    const auto& response = message.responses(0);
    EXPECT_EQ(response.request_id(), 17);
    EXPECT_EQ(ConvertTo<scada::Status>(response.status()).code(),
              scada::StatusCode::Good);
    ASSERT_EQ(response.add_node_result_size(), 1);
    EXPECT_EQ(
        ConvertTo<scada::NodeId>(response.add_node_result(0).added_node_id()),
        scada::NodeId(2, 3));
  }));

  protocol::Request request;
  request.set_request_id(17);
  Convert(std::vector<scada::AddNodesItem>{{.requested_id = {2, 3},
                                            .parent_id = {1, 1}}},
          *request.mutable_add_node());

  stub->OnRequestReceived(request);
  executor.Poll();

  ASSERT_TRUE(service.add_nodes_called);
  ASSERT_EQ(service.last_add_nodes_inputs.size(), 1u);
  EXPECT_EQ(service.last_add_nodes_inputs[0].requested_id, scada::NodeId(2, 3));
}

TEST(NodeManagementStubTest, DeleteNodesPreservesDeleteTargetReferencesFlag) {
  TestExecutor executor;
  auto sender = std::make_shared<StrictMock<MessageSenderMock>>();
  TestNodeManagementService service;

  auto stub = std::make_shared<NodeManagementStub>(
      executor, sender, service, scada::ServiceContext{}.with_user_id({1, 1}));

  EXPECT_CALL(*sender, Send(_)).WillOnce(Invoke([](protocol::Message& message) {
    ASSERT_EQ(message.responses_size(), 1);
    const auto& response = message.responses(0);
    EXPECT_EQ(response.request_id(), 23);
    EXPECT_EQ(ConvertTo<scada::Status>(response.status()).code(),
              scada::StatusCode::Good);
  }));

  protocol::Request request;
  request.set_request_id(23);
  Convert(
      std::vector<scada::DeleteNodesItem>{
          {.node_id = {7, 8}, .delete_target_references = true}},
      *request.mutable_delete_node());

  stub->OnRequestReceived(request);
  executor.Poll();

  ASSERT_TRUE(service.delete_nodes_called);
  ASSERT_EQ(service.last_delete_nodes_inputs.size(), 1u);
  EXPECT_EQ(service.last_delete_nodes_inputs[0].node_id, scada::NodeId(7, 8));
  EXPECT_TRUE(service.last_delete_nodes_inputs[0].delete_target_references);
}

}  // namespace
