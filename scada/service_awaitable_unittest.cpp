#include "scada/service_awaitable.h"

#include "base/test/awaitable_test.h"
#include "scada/attribute_service_mock.h"
#include "scada/history_service_mock.h"
#include "scada/method_service_mock.h"
#include "scada/node_management_service_mock.h"
#include "scada/view_service_mock.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace testing;

namespace scada {
namespace {

TEST(ServiceAwaitableTest, AnyExecutorReadWriteBrowseAndTranslateForwardInputs) {
  auto executor = std::make_shared<TestExecutor>();
  StrictMock<MockAttributeService> attribute_service;
  StrictMock<MockViewService> view_service;

  const auto user_id = NodeId{42, 3};
  const auto context = ServiceContext{}.with_user_id(user_id).with_request_id(17);
  auto read_inputs =
      std::make_shared<const std::vector<ReadValueId>>(std::vector<ReadValueId>{
          {.node_id = NodeId{1}, .attribute_id = AttributeId::DisplayName}});
  auto write_inputs =
      std::make_shared<const std::vector<WriteValue>>(std::vector<WriteValue>{
          {.node_id = NodeId{2},
           .attribute_id = AttributeId::Value,
           .value = Variant{Int32{9}},
           .flags = WriteFlags{}.set_select()}});
  std::vector<BrowseDescription> browse_inputs{
      {.node_id = NodeId{3},
       .direction = BrowseDirection::Forward,
       .reference_type_id = NodeId{4},
       .include_subtypes = false}};
  std::vector<BrowsePath> translate_inputs{
      {.node_id = NodeId{5},
       .relative_path = {{.reference_type_id = NodeId{6},
                          .target_name = {"Leaf", 7}}}}};

  EXPECT_CALL(attribute_service, Read(_, _, _))
      .WillOnce(Invoke([&](const ServiceContext& actual_context,
                           const std::shared_ptr<const std::vector<ReadValueId>>& actual_inputs,
                           const ReadCallback& callback) {
        EXPECT_EQ(actual_context.user_id(), user_id);
        EXPECT_EQ(actual_context.request_id(), 17u);
        EXPECT_EQ(actual_inputs, read_inputs);
        callback(StatusCode::Good,
                 {DataValue{LocalizedText{u"Pump"}, {}, {}, {}}});
      }));
  EXPECT_CALL(attribute_service, Write(_, _, _))
      .WillOnce(Invoke([&](const ServiceContext& actual_context,
                           const std::shared_ptr<const std::vector<WriteValue>>& actual_inputs,
                           const WriteCallback& callback) {
        EXPECT_EQ(actual_context.user_id(), user_id);
        EXPECT_EQ(actual_context.request_id(), 17u);
        EXPECT_EQ(actual_inputs, write_inputs);
        callback(StatusCode::Good, {StatusCode::Good});
      }));
  EXPECT_CALL(view_service, Browse(_, _, _))
      .WillOnce(Invoke([&](const ServiceContext& actual_context,
                           const std::vector<BrowseDescription>& actual_inputs,
                           const BrowseCallback& callback) {
        EXPECT_EQ(actual_context.user_id(), user_id);
        EXPECT_EQ(actual_context.request_id(), 17u);
        EXPECT_THAT(actual_inputs, ElementsAre(browse_inputs[0]));
        callback(StatusCode::Good,
                 {BrowseResult{
                     .status_code = StatusCode::Good,
                     .references = {{.reference_type_id = NodeId{7},
                                     .forward = true,
                                     .node_id = NodeId{8}}}}});
      }));
  EXPECT_CALL(view_service, TranslateBrowsePaths(_, _))
      .WillOnce(Invoke([&](const std::vector<BrowsePath>& actual_inputs,
                           const TranslateBrowsePathsCallback& callback) {
        EXPECT_THAT(actual_inputs, ElementsAre(translate_inputs[0]));
        callback(StatusCode::Good,
                 {BrowsePathResult{
                     .status_code = StatusCode::Good,
                     .targets = {{.target_id = ExpandedNodeId{NodeId{9}},
                                  .remaining_path_index = 0}}}});
      }));

  auto [read_status, read_results] = WaitAwaitable(
      executor,
      ReadAsync(MakeTestAnyExecutor(executor), attribute_service, context,
                read_inputs));
  EXPECT_EQ(read_status.code(), StatusCode::Good);
  ASSERT_EQ(read_results.size(), 1u);
  EXPECT_EQ(read_results[0].value, Variant{LocalizedText{u"Pump"}});

  auto [write_status, write_results] = WaitAwaitable(
      executor,
      WriteAsync(MakeTestAnyExecutor(executor), attribute_service, context,
                 write_inputs));
  EXPECT_EQ(write_status.code(), StatusCode::Good);
  EXPECT_THAT(write_results, ElementsAre(StatusCode::Good));

  auto [browse_status, browse_results] = WaitAwaitable(
      executor,
      BrowseAsync(MakeTestAnyExecutor(executor), view_service, context,
                  std::move(browse_inputs)));
  EXPECT_EQ(browse_status.code(), StatusCode::Good);
  ASSERT_EQ(browse_results.size(), 1u);
  EXPECT_THAT(browse_results[0].references,
              ElementsAre(ReferenceDescription{
                  .reference_type_id = NodeId{7},
                  .forward = true,
                  .node_id = NodeId{8}}));

  auto [translate_status, translate_results] =
      WaitAwaitable(executor,
                    TranslateBrowsePathsAsync(MakeTestAnyExecutor(executor),
                                              view_service,
                                              std::move(translate_inputs)));
  EXPECT_EQ(translate_status.code(), StatusCode::Good);
  ASSERT_EQ(translate_results.size(), 1u);
  ASSERT_EQ(translate_results[0].targets.size(), 1u);
  EXPECT_EQ(translate_results[0].targets[0].target_id,
            ExpandedNodeId{NodeId{9}});
}

TEST(ServiceAwaitableTest,
     LegacyExecutorCallHistoryAndNodeManagementForwardInputs) {
  auto executor = std::make_shared<TestExecutor>();
  StrictMock<MockMethodService> method_service;
  StrictMock<MockHistoryService> history_service;
  StrictMock<MockNodeManagementService> node_management_service;

  const auto node_id = NodeId{10};
  const auto method_id = NodeId{11};
  std::vector<Variant> arguments{Variant{true}, Variant{Int32{5}}};
  const auto user_id = NodeId{12};
  const auto from = base::Time::Now() - base::TimeDelta::FromHours(2);
  const auto to = base::Time::Now();
  HistoryReadRawDetails raw_details{
      .node_id = NodeId{13}, .from = from, .to = to, .max_count = 25};
  EventFilter filter;
  filter.types = EventFilter::UNACKED;
  filter.add_of_type(NodeId{30}).add_child_of(NodeId{31});
  std::vector<AddNodesItem> add_nodes_inputs{
      {.requested_id = NodeId{14},
       .parent_id = NodeId{15},
       .type_definition_id = NodeId{16}}};
  std::vector<DeleteNodesItem> delete_nodes_inputs{
      {.node_id = NodeId{17}, .delete_target_references = true}};
  std::vector<AddReferencesItem> add_references_inputs{
      {.source_node_id = NodeId{18},
       .reference_type_id = NodeId{19},
       .target_node_id = ExpandedNodeId{NodeId{20}}}};
  std::vector<DeleteReferencesItem> delete_references_inputs{
      {.source_node_id = NodeId{21},
       .reference_type_id = NodeId{22},
       .target_node_id = ExpandedNodeId{NodeId{23}}}};

  EXPECT_CALL(method_service, Call(node_id, method_id, arguments, user_id, _))
      .WillOnce(Invoke([](const NodeId&, const NodeId&,
                          const std::vector<Variant>&, const NodeId&,
                          const StatusCallback& callback) {
        callback(StatusCode::Bad_WrongCallArguments);
      }));
  EXPECT_CALL(history_service, HistoryReadRaw(_, _))
      .WillOnce(Invoke([&](const HistoryReadRawDetails& actual_details,
                          const HistoryReadRawCallback& callback) {
        EXPECT_EQ(actual_details.node_id, raw_details.node_id);
        EXPECT_EQ(actual_details.from, raw_details.from);
        EXPECT_EQ(actual_details.to, raw_details.to);
        EXPECT_EQ(actual_details.max_count, raw_details.max_count);
        callback(HistoryReadRawResult{
            .status = StatusCode::Good,
            .values = {DataValue{Variant{12.5}, {}, {}, {}}},
            .continuation_point = {1, 2, 3},
        });
      }));
  EXPECT_CALL(history_service, HistoryReadEvents(node_id, from, to, _, _))
      .WillOnce(Invoke([&](const NodeId&, base::Time, base::Time,
                           const EventFilter& actual_filter,
                           const HistoryReadEventsCallback& callback) {
        EXPECT_EQ(actual_filter, filter);
        callback(HistoryReadEventsResult{.status = StatusCode::Bad_Disconnected});
      }));
  EXPECT_CALL(node_management_service, AddNodes(_, _))
      .WillOnce(Invoke([&](const std::vector<AddNodesItem>& actual_inputs,
                          const AddNodesCallback& callback) {
        ASSERT_EQ(actual_inputs.size(), 1u);
        EXPECT_EQ(actual_inputs[0].requested_id, add_nodes_inputs[0].requested_id);
        EXPECT_EQ(actual_inputs[0].parent_id, add_nodes_inputs[0].parent_id);
        EXPECT_EQ(actual_inputs[0].type_definition_id,
                  add_nodes_inputs[0].type_definition_id);
        callback(StatusCode::Good,
                 {AddNodesResult{
                     .status_code = StatusCode::Good,
                     .added_node_id = NodeId{24}}});
      }));
  EXPECT_CALL(node_management_service, DeleteNodes(_, _))
      .WillOnce(Invoke([&](const std::vector<DeleteNodesItem>& actual_inputs,
                          const DeleteNodesCallback& callback) {
        ASSERT_EQ(actual_inputs.size(), 1u);
        EXPECT_EQ(actual_inputs[0].node_id, delete_nodes_inputs[0].node_id);
        EXPECT_EQ(actual_inputs[0].delete_target_references,
                  delete_nodes_inputs[0].delete_target_references);
        callback(StatusCode::Good, {StatusCode::Bad_WrongNodeId});
      }));
  EXPECT_CALL(node_management_service, AddReferences(_, _))
      .WillOnce(Invoke([&](const std::vector<AddReferencesItem>& actual_inputs,
                          const AddReferencesCallback& callback) {
        ASSERT_EQ(actual_inputs.size(), 1u);
        EXPECT_EQ(actual_inputs[0].source_node_id,
                  add_references_inputs[0].source_node_id);
        EXPECT_EQ(actual_inputs[0].reference_type_id,
                  add_references_inputs[0].reference_type_id);
        EXPECT_EQ(actual_inputs[0].target_node_id,
                  add_references_inputs[0].target_node_id);
        callback(StatusCode::Good, {StatusCode::Bad_WrongTargetId});
      }));
  EXPECT_CALL(node_management_service, DeleteReferences(_, _))
      .WillOnce(Invoke([&](const std::vector<DeleteReferencesItem>& actual_inputs,
                          const DeleteReferencesCallback& callback) {
        ASSERT_EQ(actual_inputs.size(), 1u);
        EXPECT_EQ(actual_inputs[0].source_node_id,
                  delete_references_inputs[0].source_node_id);
        EXPECT_EQ(actual_inputs[0].reference_type_id,
                  delete_references_inputs[0].reference_type_id);
        EXPECT_EQ(actual_inputs[0].target_node_id,
                  delete_references_inputs[0].target_node_id);
        callback(StatusCode::Bad_Disconnected,
                 {StatusCode::Bad_Disconnected});
      }));

  const auto call_status = WaitAwaitable(
      executor,
      CallAsync(executor, method_service, node_id, method_id, arguments,
                user_id));
  EXPECT_EQ(call_status.code(), StatusCode::Bad_WrongCallArguments);

  const auto raw_result = WaitAwaitable(
      executor, HistoryReadRawAsync(executor, history_service, raw_details));
  EXPECT_EQ(raw_result.status.code(), StatusCode::Good);
  ASSERT_EQ(raw_result.values.size(), 1u);
  EXPECT_EQ(raw_result.continuation_point, (ByteString{1, 2, 3}));

  const auto events_result = WaitAwaitable(
      executor,
      HistoryReadEventsAsync(executor, history_service, node_id, from, to,
                             std::move(filter)));
  EXPECT_EQ(events_result.status.code(), StatusCode::Bad_Disconnected);

  auto [add_nodes_status, add_nodes_results] = WaitAwaitable(
      executor,
      AddNodesAsync(executor, node_management_service,
                    std::move(add_nodes_inputs)));
  EXPECT_EQ(add_nodes_status.code(), StatusCode::Good);
  ASSERT_EQ(add_nodes_results.size(), 1u);
  EXPECT_EQ(add_nodes_results[0].added_node_id, NodeId{24});

  auto [delete_nodes_status, delete_nodes_results] = WaitAwaitable(
      executor,
      DeleteNodesAsync(executor, node_management_service,
                       std::move(delete_nodes_inputs)));
  EXPECT_EQ(delete_nodes_status.code(), StatusCode::Good);
  EXPECT_THAT(delete_nodes_results, ElementsAre(StatusCode::Bad_WrongNodeId));

  auto [add_references_status, add_references_results] = WaitAwaitable(
      executor,
      AddReferencesAsync(executor, node_management_service,
                         std::move(add_references_inputs)));
  EXPECT_EQ(add_references_status.code(), StatusCode::Good);
  EXPECT_THAT(add_references_results,
              ElementsAre(StatusCode::Bad_WrongTargetId));

  auto [delete_references_status, delete_references_results] = WaitAwaitable(
      executor,
      DeleteReferencesAsync(executor, node_management_service,
                            std::move(delete_references_inputs)));
  EXPECT_EQ(delete_references_status.code(), StatusCode::Bad_Disconnected);
  EXPECT_THAT(delete_references_results,
              ElementsAre(StatusCode::Bad_Disconnected));
}

}  // namespace
}  // namespace scada
