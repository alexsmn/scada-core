#include "scada/coroutine_services.h"

#include "base/test/test_executor.h"
#include "scada/attribute_service_mock.h"
#include "scada/history_service_mock.h"
#include "scada/method_service_mock.h"
#include "scada/node_management_service_mock.h"
#include "scada/session_service_mock.h"
#include "scada/status_promise.h"
#include "scada/view_service_mock.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace std::chrono_literals;

namespace scada {
namespace {

using testing::_;
using testing::Invoke;

template <class T>
T WaitForPromise(const std::shared_ptr<TestExecutor>& executor, promise<T>& p) {
  while (p.wait_for(0ms) == promise_wait_status::timeout) {
    executor->Poll();
  }
  return p.get();
}

void WaitForPromise(const std::shared_ptr<TestExecutor>& executor,
                    promise<void>& p) {
  while (p.wait_for(0ms) == promise_wait_status::timeout) {
    executor->Poll();
  }
  p.get();
}

class TestCoroutineAttributeService final : public CoroutineAttributeService {
 public:
  Awaitable<std::tuple<Status, std::vector<DataValue>>> Read(
      ServiceContext context,
      std::shared_ptr<const std::vector<ReadValueId>> inputs) override {
    last_read_context = std::move(context);
    last_read_inputs = std::move(inputs);
    if (throw_on_read) {
      throw status_exception{StatusCode::Bad};
    }
    co_return read_result;
  }

  Awaitable<std::tuple<Status, std::vector<StatusCode>>> Write(
      ServiceContext context,
      std::shared_ptr<const std::vector<WriteValue>> inputs) override {
    last_write_context = std::move(context);
    last_write_inputs = std::move(inputs);
    co_return write_result;
  }

  ServiceContext last_read_context;
  std::shared_ptr<const std::vector<ReadValueId>> last_read_inputs;
  ServiceContext last_write_context;
  std::shared_ptr<const std::vector<WriteValue>> last_write_inputs;
  bool throw_on_read = false;
  std::tuple<Status, std::vector<DataValue>> read_result{
      StatusCode::Good, {DataValue{Variant{42}, {}, {}, {}}}};
  std::tuple<Status, std::vector<StatusCode>> write_result{
      StatusCode::Good, {StatusCode::Good}};
};

class TestCoroutineMethodService final : public CoroutineMethodService {
 public:
  Awaitable<Status> Call(NodeId node_id,
                         NodeId method_id,
                         std::vector<Variant> arguments,
                         NodeId user_id) override {
    last_node_id = std::move(node_id);
    last_method_id = std::move(method_id);
    last_arguments = std::move(arguments);
    last_user_id = std::move(user_id);
    co_return status;
  }

  NodeId last_node_id;
  NodeId last_method_id;
  std::vector<Variant> last_arguments;
  NodeId last_user_id;
  Status status{StatusCode::Good};
};

class TestCoroutineHistoryService final : public CoroutineHistoryService {
 public:
  Awaitable<HistoryReadRawResult> HistoryReadRaw(
      HistoryReadRawDetails details) override {
    last_raw_details = std::move(details);
    co_return raw_result;
  }

  Awaitable<HistoryReadEventsResult> HistoryReadEvents(
      NodeId node_id,
      base::Time from,
      base::Time to,
      EventFilter filter) override {
    last_node_id = std::move(node_id);
    last_from = from;
    last_to = to;
    last_filter = std::move(filter);
    co_return events_result;
  }

  HistoryReadRawDetails last_raw_details;
  NodeId last_node_id;
  base::Time last_from;
  base::Time last_to;
  EventFilter last_filter;
  HistoryReadRawResult raw_result{
      .status = StatusCode::Good,
      .values = {DataValue{Variant{7}, {}, {}, {}}},
      .continuation_point = {1, 2, 3}};
  HistoryReadEventsResult events_result{.status = StatusCode::Good};
};

class TestCoroutineViewService final : public CoroutineViewService {
 public:
  Awaitable<std::tuple<Status, std::vector<BrowseResult>>> Browse(
      ServiceContext context,
      std::vector<BrowseDescription> inputs) override {
    last_browse_context = std::move(context);
    last_browse_inputs = std::move(inputs);
    co_return browse_result;
  }

  Awaitable<std::tuple<Status, std::vector<BrowsePathResult>>>
  TranslateBrowsePaths(std::vector<BrowsePath> inputs) override {
    last_translate_inputs = std::move(inputs);
    co_return translate_result;
  }

  ServiceContext last_browse_context;
  std::vector<BrowseDescription> last_browse_inputs;
  std::vector<BrowsePath> last_translate_inputs;
  std::tuple<Status, std::vector<BrowseResult>> browse_result{
      StatusCode::Good,
      {BrowseResult{
          .status_code = StatusCode::Good,
          .references = {{.reference_type_id = NodeId{2},
                          .forward = true,
                          .node_id = NodeId{3}}}}}};
  std::tuple<Status, std::vector<BrowsePathResult>> translate_result{
      StatusCode::Good,
      {BrowsePathResult{
          .status_code = StatusCode::Good,
          .targets = {{.target_id = ExpandedNodeId{NodeId{4}},
                       .remaining_path_index = 0}}}}};
};

class TestCoroutineNodeManagementService final
    : public CoroutineNodeManagementService {
 public:
  Awaitable<std::tuple<Status, std::vector<AddNodesResult>>> AddNodes(
      std::vector<AddNodesItem> inputs) override {
    last_add_nodes_inputs = std::move(inputs);
    co_return add_nodes_result;
  }

  Awaitable<std::tuple<Status, std::vector<StatusCode>>> DeleteNodes(
      std::vector<DeleteNodesItem> inputs) override {
    last_delete_nodes_inputs = std::move(inputs);
    co_return delete_nodes_result;
  }

  Awaitable<std::tuple<Status, std::vector<StatusCode>>> AddReferences(
      std::vector<AddReferencesItem> inputs) override {
    last_add_references_inputs = std::move(inputs);
    co_return add_references_result;
  }

  Awaitable<std::tuple<Status, std::vector<StatusCode>>> DeleteReferences(
      std::vector<DeleteReferencesItem> inputs) override {
    last_delete_references_inputs = std::move(inputs);
    co_return delete_references_result;
  }

  std::vector<AddNodesItem> last_add_nodes_inputs;
  std::vector<DeleteNodesItem> last_delete_nodes_inputs;
  std::vector<AddReferencesItem> last_add_references_inputs;
  std::vector<DeleteReferencesItem> last_delete_references_inputs;
  std::tuple<Status, std::vector<AddNodesResult>> add_nodes_result{
      StatusCode::Good,
      {AddNodesResult{.status_code = StatusCode::Good,
                      .added_node_id = NodeId{5}}}};
  std::tuple<Status, std::vector<StatusCode>> delete_nodes_result{
      StatusCode::Good, {StatusCode::Good}};
  std::tuple<Status, std::vector<StatusCode>> add_references_result{
      StatusCode::Good, {StatusCode::Good}};
  std::tuple<Status, std::vector<StatusCode>> delete_references_result{
      StatusCode::Good, {StatusCode::Good}};
};

class TestCoroutineSessionService final : public CoroutineSessionService {
 public:
  Awaitable<void> Connect(SessionConnectParams params) override {
    last_connect_params = std::move(params);
    connected = true;
    co_return;
  }

  Awaitable<void> Reconnect() override {
    reconnect_called = true;
    co_return;
  }

  Awaitable<void> Disconnect() override {
    disconnect_called = true;
    connected = false;
    co_return;
  }

  bool IsConnected(base::TimeDelta* ping_delay = nullptr) const override {
    return connected;
  }

  NodeId GetUserId() const override { return user_id; }

  bool HasPrivilege(Privilege privilege) const override {
    return privilege == allowed_privilege;
  }

  std::string GetHostName() const override { return host_name; }

  bool IsScada() const override { return is_scada; }

  boost::signals2::scoped_connection SubscribeSessionStateChanged(
      const SessionStateChangedCallback& callback) override {
    state_changed_callback = callback;
    return {};
  }

  SessionDebugger* GetSessionDebugger() override { return debugger; }

  SessionConnectParams last_connect_params;
  SessionStateChangedCallback state_changed_callback;
  SessionDebugger* debugger = nullptr;
  NodeId user_id{9};
  Privilege allowed_privilege = Privilege::Control;
  std::string host_name = "test-host";
  bool connected = false;
  bool is_scada = true;
  bool reconnect_called = false;
  bool disconnect_called = false;
};

TEST(CallbackToCoroutineAttributeServiceAdapter, ReadAndWriteForwardResults) {
  auto executor = std::make_shared<TestExecutor>();
  testing::StrictMock<MockAttributeService> service;
  CallbackToCoroutineAttributeServiceAdapter adapter{executor, service};

  const auto context = ServiceContext{}.with_request_id(17);
  auto read_inputs =
      std::make_shared<std::vector<ReadValueId>>(1, ReadValueId{.node_id = 1});
  auto write_inputs = std::make_shared<std::vector<WriteValue>>(
      1, WriteValue{.node_id = 2, .value = Variant{11}});

  EXPECT_CALL(service, Read(_, _, _))
      .WillOnce(Invoke([&](const ServiceContext& actual_context,
                           const std::shared_ptr<const std::vector<ReadValueId>>&
                               actual_inputs,
                           const ReadCallback& callback) {
        EXPECT_EQ(actual_context.request_id(), context.request_id());
        ASSERT_EQ(actual_inputs->size(), 1u);
        EXPECT_EQ((*actual_inputs)[0], (*read_inputs)[0]);
        callback(StatusCode::Good, {DataValue{Variant{42}, {}, {}, {}}});
      }));

  auto read_promise =
      ToPromise(NetExecutorAdapter{executor}, adapter.Read(context, read_inputs));
  auto [read_status, read_results] = WaitForPromise(executor, read_promise);
  EXPECT_TRUE(read_status);
  ASSERT_EQ(read_results.size(), 1u);
  EXPECT_EQ(read_results[0], DataValue(Variant{42}, {}, {}, {}));

  EXPECT_CALL(service, Write(_, _, _))
      .WillOnce(Invoke([&](const ServiceContext& actual_context,
                           const std::shared_ptr<const std::vector<WriteValue>>&
                               actual_inputs,
                           const WriteCallback& callback) {
        EXPECT_EQ(actual_context.request_id(), context.request_id());
        ASSERT_EQ(actual_inputs->size(), 1u);
        EXPECT_EQ((*actual_inputs)[0], (*write_inputs)[0]);
        callback(StatusCode::Good, {StatusCode::Good});
      }));

  auto write_promise = ToPromise(NetExecutorAdapter{executor},
                                 adapter.Write(context, write_inputs));
  auto [write_status, write_results] = WaitForPromise(executor, write_promise);
  EXPECT_TRUE(write_status);
  EXPECT_THAT(write_results, testing::ElementsAre(StatusCode::Good));
}

TEST(CallbackToCoroutineMethodServiceAdapter, CallForwardsArguments) {
  auto executor = std::make_shared<TestExecutor>();
  testing::StrictMock<MockMethodService> service;
  CallbackToCoroutineMethodServiceAdapter adapter{executor, service};

  EXPECT_CALL(service, Call(NodeId{1}, NodeId{2}, testing::ElementsAre(Variant{3}),
                            NodeId{4}, _))
      .WillOnce(Invoke([](const NodeId&, const NodeId&,
                          const std::vector<Variant>&, const NodeId&,
                          const StatusCallback& callback) {
        callback(StatusCode::Good);
      }));

  auto promise = ToPromise(NetExecutorAdapter{executor},
                           adapter.Call(NodeId{1}, NodeId{2},
                                        {Variant{3}}, NodeId{4}));

  EXPECT_TRUE(WaitForPromise(executor, promise));
}

TEST(CallbackToCoroutineHistoryServiceAdapter, ForwardsHistoryReads) {
  auto executor = std::make_shared<TestExecutor>();
  testing::StrictMock<MockHistoryService> service;
  CallbackToCoroutineHistoryServiceAdapter adapter{executor, service};

  HistoryReadRawDetails raw_details{.node_id = NodeId{1}, .max_count = 2};
  EXPECT_CALL(service, HistoryReadRaw(_, _))
      .WillOnce(Invoke([&](const HistoryReadRawDetails& actual_details,
                           const HistoryReadRawCallback& callback) {
        EXPECT_EQ(actual_details.node_id, raw_details.node_id);
        EXPECT_EQ(actual_details.max_count, raw_details.max_count);
        callback(HistoryReadRawResult{
            .status = StatusCode::Good,
            .values = {DataValue{Variant{7}, {}, {}, {}}},
            .continuation_point = {1}});
      }));

  auto raw_promise =
      ToPromise(NetExecutorAdapter{executor}, adapter.HistoryReadRaw(raw_details));
  auto raw_result = WaitForPromise(executor, raw_promise);
  EXPECT_TRUE(raw_result.status);
  ASSERT_EQ(raw_result.values.size(), 1u);
  EXPECT_EQ(raw_result.values[0], DataValue(Variant{7}, {}, {}, {}));

  EXPECT_CALL(service, HistoryReadEvents(NodeId{3}, _, _, _, _))
      .WillOnce(Invoke([](const NodeId&, base::Time, base::Time,
                          const EventFilter&, const HistoryReadEventsCallback& cb) {
        cb(HistoryReadEventsResult{.status = StatusCode::Good});
      }));

  auto events_promise = ToPromise(NetExecutorAdapter{executor},
                                  adapter.HistoryReadEvents(NodeId{3}, {}, {},
                                                            {}));
  EXPECT_TRUE(WaitForPromise(executor, events_promise).status);
}

TEST(CallbackToCoroutineViewServiceAdapter, ForwardsBrowseOperations) {
  auto executor = std::make_shared<TestExecutor>();
  testing::StrictMock<MockViewService> service;
  CallbackToCoroutineViewServiceAdapter adapter{executor, service};

  const auto context = ServiceContext{}.with_request_id(21);
  const std::vector<BrowseDescription> browse_inputs{{.node_id = NodeId{1}}};

  EXPECT_CALL(service, Browse(_, browse_inputs, _))
      .WillOnce(Invoke([&](const ServiceContext& actual_context,
                           const std::vector<BrowseDescription>&,
                           const BrowseCallback& callback) {
        EXPECT_EQ(actual_context.request_id(), context.request_id());
        callback(StatusCode::Good,
                 {BrowseResult{
                     .status_code = StatusCode::Good,
                     .references = {{.reference_type_id = NodeId{2},
                                     .forward = true,
                                     .node_id = NodeId{3}}}}});
      }));

  auto browse_promise =
      ToPromise(NetExecutorAdapter{executor}, adapter.Browse(context, browse_inputs));
  auto [browse_status, browse_results] =
      WaitForPromise(executor, browse_promise);
  EXPECT_TRUE(browse_status);
  ASSERT_EQ(browse_results.size(), 1u);
  EXPECT_EQ(browse_results[0].references[0].node_id, NodeId{3});

  const std::vector<BrowsePath> translate_inputs{
      {.node_id = NodeId{4}, .relative_path = {}}};
  EXPECT_CALL(service, TranslateBrowsePaths(translate_inputs, _))
      .WillOnce(Invoke([](const std::vector<BrowsePath>&,
                          const TranslateBrowsePathsCallback& callback) {
        callback(StatusCode::Good,
                 {BrowsePathResult{
                     .status_code = StatusCode::Good,
                     .targets = {{.target_id = ExpandedNodeId{NodeId{5}},
                                  .remaining_path_index = 0}}}});
      }));

  auto translate_promise = ToPromise(NetExecutorAdapter{executor},
                                     adapter.TranslateBrowsePaths(
                                         translate_inputs));
  auto [translate_status, translate_results] =
      WaitForPromise(executor, translate_promise);
  EXPECT_TRUE(translate_status);
  ASSERT_EQ(translate_results.size(), 1u);
  EXPECT_EQ(translate_results[0].targets[0].target_id.node_id(), NodeId{5});
}

TEST(CallbackToCoroutineNodeManagementServiceAdapter,
     ForwardsNodeManagementOperations) {
  auto executor = std::make_shared<TestExecutor>();
  testing::StrictMock<MockNodeManagementService> service;
  CallbackToCoroutineNodeManagementServiceAdapter adapter{executor, service};

  const std::vector<AddNodesItem> add_nodes_inputs{
      {.requested_id = NodeId{1}, .parent_id = NodeId{2}}};
  EXPECT_CALL(service, AddNodes(add_nodes_inputs, _))
      .WillOnce(Invoke([](const std::vector<AddNodesItem>&,
                          const AddNodesCallback& callback) {
        callback(StatusCode::Good,
                 {AddNodesResult{.status_code = StatusCode::Good,
                                 .added_node_id = NodeId{3}}});
      }));

  auto add_nodes_promise = ToPromise(NetExecutorAdapter{executor},
                                     adapter.AddNodes(add_nodes_inputs));
  auto [add_nodes_status, add_nodes_results] =
      WaitForPromise(executor, add_nodes_promise);
  EXPECT_TRUE(add_nodes_status);
  EXPECT_EQ(add_nodes_results[0].added_node_id, NodeId{3});

  const std::vector<DeleteNodesItem> delete_nodes_inputs{
      {.node_id = NodeId{4}, .delete_target_references = true}};
  EXPECT_CALL(service, DeleteNodes(_, _))
      .WillOnce(Invoke([&](const std::vector<DeleteNodesItem>& actual_inputs,
                          const DeleteNodesCallback& callback) {
        ASSERT_EQ(actual_inputs.size(), delete_nodes_inputs.size());
        EXPECT_EQ(actual_inputs[0].node_id, delete_nodes_inputs[0].node_id);
        EXPECT_EQ(actual_inputs[0].delete_target_references,
                  delete_nodes_inputs[0].delete_target_references);
        callback(StatusCode::Good, {StatusCode::Good});
      }));

  auto delete_nodes_promise = ToPromise(NetExecutorAdapter{executor},
                                        adapter.DeleteNodes(delete_nodes_inputs));
  auto [delete_nodes_status, delete_nodes_results] =
      WaitForPromise(executor, delete_nodes_promise);
  EXPECT_TRUE(delete_nodes_status);
  EXPECT_THAT(delete_nodes_results, testing::ElementsAre(StatusCode::Good));

  const std::vector<AddReferencesItem> add_references_inputs{
      {.source_node_id = NodeId{5},
       .reference_type_id = NodeId{6},
       .target_node_id = ExpandedNodeId{NodeId{7}}}};
  EXPECT_CALL(service, AddReferences(_, _))
      .WillOnce(Invoke([&](const std::vector<AddReferencesItem>& actual_inputs,
                          const AddReferencesCallback& callback) {
        ASSERT_EQ(actual_inputs.size(), add_references_inputs.size());
        EXPECT_EQ(actual_inputs[0].source_node_id,
                  add_references_inputs[0].source_node_id);
        EXPECT_EQ(actual_inputs[0].reference_type_id,
                  add_references_inputs[0].reference_type_id);
        EXPECT_EQ(actual_inputs[0].target_node_id,
                  add_references_inputs[0].target_node_id);
        callback(StatusCode::Good, {StatusCode::Good});
      }));

  auto add_references_promise = ToPromise(NetExecutorAdapter{executor},
                                          adapter.AddReferences(
                                              add_references_inputs));
  auto [add_references_status, add_references_results] =
      WaitForPromise(executor, add_references_promise);
  EXPECT_TRUE(add_references_status);
  EXPECT_THAT(add_references_results, testing::ElementsAre(StatusCode::Good));

  const std::vector<DeleteReferencesItem> delete_references_inputs{
      {.source_node_id = NodeId{8},
       .reference_type_id = NodeId{9},
       .target_node_id = ExpandedNodeId{NodeId{10}}}};
  EXPECT_CALL(service, DeleteReferences(_, _))
      .WillOnce(Invoke([&](const std::vector<DeleteReferencesItem>& actual_inputs,
                          const DeleteReferencesCallback& callback) {
        ASSERT_EQ(actual_inputs.size(), delete_references_inputs.size());
        EXPECT_EQ(actual_inputs[0].source_node_id,
                  delete_references_inputs[0].source_node_id);
        EXPECT_EQ(actual_inputs[0].reference_type_id,
                  delete_references_inputs[0].reference_type_id);
        EXPECT_EQ(actual_inputs[0].target_node_id,
                  delete_references_inputs[0].target_node_id);
        callback(StatusCode::Good, {StatusCode::Good});
      }));

  auto delete_references_promise = ToPromise(
      NetExecutorAdapter{executor},
      adapter.DeleteReferences(delete_references_inputs));
  auto [delete_references_status, delete_references_results] =
      WaitForPromise(executor, delete_references_promise);
  EXPECT_TRUE(delete_references_status);
  EXPECT_THAT(delete_references_results, testing::ElementsAre(StatusCode::Good));
}

TEST(PromiseToCoroutineSessionServiceAdapter, ConnectsAndPassesThroughState) {
  auto executor = std::make_shared<TestExecutor>();
  testing::StrictMock<MockSessionService> service;
  PromiseToCoroutineSessionServiceAdapter adapter{executor, service};

  SessionConnectParams params{.host = "opc.tcp://localhost:4840"};
  EXPECT_CALL(service, Connect(_))
      .WillOnce(Invoke([&](const SessionConnectParams& actual_params) {
        EXPECT_EQ(actual_params.host, params.host);
        promise<void> result;
        Dispatch(*executor, [result]() mutable { result.resolve(); });
        return result;
      }));

  auto promise =
      ToPromise(NetExecutorAdapter{executor}, adapter.Connect(params));
  EXPECT_NO_THROW(WaitForPromise(executor, promise));

  EXPECT_CALL(service, IsConnected(_)).WillOnce(Invoke([](base::TimeDelta*) {
    return true;
  }));
  EXPECT_TRUE(adapter.IsConnected());
}

TEST(CoroutineToCallbackAttributeServiceAdapter, ReadAndWriteForwardResults) {
  auto executor = std::make_shared<TestExecutor>();
  TestCoroutineAttributeService service;
  CoroutineToCallbackAttributeServiceAdapter adapter{executor, service};

  promise<DataValue> read_result;
  const auto context = ServiceContext{}.with_request_id(33);
  auto read_inputs =
      std::make_shared<std::vector<ReadValueId>>(1, ReadValueId{.node_id = 1});
  adapter.Read(
      context, read_inputs,
      [read_result](Status status, std::vector<DataValue> results) mutable {
        EXPECT_TRUE(status);
        EXPECT_EQ(results.size(), 1u);
        read_result.resolve(std::move(results[0]));
      });
  EXPECT_EQ(WaitForPromise(executor, read_result),
            DataValue(Variant{42}, {}, {}, {}));
  EXPECT_EQ(service.last_read_context.request_id(), context.request_id());
  ASSERT_TRUE(service.last_read_inputs);
  EXPECT_EQ(*service.last_read_inputs, *read_inputs);

  promise<std::vector<StatusCode>> write_result;
  auto write_inputs = std::make_shared<std::vector<WriteValue>>(
      1, WriteValue{.node_id = 2, .value = Variant{12}});
  adapter.Write(
      context, write_inputs,
      [write_result](Status&& status, std::vector<StatusCode>&& results) mutable {
        EXPECT_TRUE(status);
        write_result.resolve(std::move(results));
      });
  EXPECT_THAT(WaitForPromise(executor, write_result),
              testing::ElementsAre(StatusCode::Good));
  EXPECT_EQ(service.last_write_context.request_id(), context.request_id());
  ASSERT_TRUE(service.last_write_inputs);
  EXPECT_EQ(*service.last_write_inputs, *write_inputs);
}

TEST(CoroutineToCallbackAttributeServiceAdapter,
     ConvertsCoroutineExceptionsToBadStatus) {
  auto executor = std::make_shared<TestExecutor>();
  TestCoroutineAttributeService service;
  service.throw_on_read = true;
  CoroutineToCallbackAttributeServiceAdapter adapter{executor, service};

  promise<StatusCode> status_result;
  auto inputs =
      std::make_shared<std::vector<ReadValueId>>(1, ReadValueId{.node_id = 1});
  adapter.Read(ServiceContext{}, inputs,
               [status_result](Status status, std::vector<DataValue> results) mutable {
                 EXPECT_TRUE(results.empty());
                 status_result.resolve(status.code());
               });

  EXPECT_EQ(WaitForPromise(executor, status_result), StatusCode::Bad);
}

TEST(CoroutineToCallbackMethodServiceAdapter, CallForwardsResults) {
  auto executor = std::make_shared<TestExecutor>();
  TestCoroutineMethodService service;
  CoroutineToCallbackMethodServiceAdapter adapter{executor, service};

  promise<StatusCode> result;
  adapter.Call(NodeId{1}, NodeId{2}, {Variant{3}}, NodeId{4},
               [result](Status&& status) mutable { result.resolve(status.code()); });

  EXPECT_EQ(WaitForPromise(executor, result), StatusCode::Good);
  EXPECT_EQ(service.last_node_id, NodeId{1});
  EXPECT_EQ(service.last_method_id, NodeId{2});
  EXPECT_THAT(service.last_arguments, testing::ElementsAre(Variant{3}));
  EXPECT_EQ(service.last_user_id, NodeId{4});
}

TEST(CoroutineToCallbackHistoryServiceAdapter, ForwardsHistoryReads) {
  auto executor = std::make_shared<TestExecutor>();
  TestCoroutineHistoryService service;
  CoroutineToCallbackHistoryServiceAdapter adapter{executor, service};

  promise<HistoryReadRawResult> raw_promise;
  HistoryReadRawDetails raw_details{.node_id = NodeId{1}, .max_count = 5};
  adapter.HistoryReadRaw(raw_details,
                         [raw_promise](HistoryReadRawResult result) mutable {
                           raw_promise.resolve(std::move(result));
                         });
  auto raw_result = WaitForPromise(executor, raw_promise);
  EXPECT_TRUE(raw_result.status);
  EXPECT_EQ(service.last_raw_details.node_id, raw_details.node_id);
  EXPECT_EQ(service.last_raw_details.max_count, raw_details.max_count);

  promise<HistoryReadEventsResult> events_promise;
  adapter.HistoryReadEvents(NodeId{2}, {}, {}, {},
                            [events_promise](HistoryReadEventsResult result) mutable {
                              events_promise.resolve(std::move(result));
                            });
  EXPECT_TRUE(WaitForPromise(executor, events_promise).status);
  EXPECT_EQ(service.last_node_id, NodeId{2});
}

TEST(CoroutineToCallbackViewServiceAdapter, ForwardsBrowseOperations) {
  auto executor = std::make_shared<TestExecutor>();
  TestCoroutineViewService service;
  CoroutineToCallbackViewServiceAdapter adapter{executor, service};

  promise<std::vector<BrowseResult>> browse_promise;
  const auto context = ServiceContext{}.with_request_id(55);
  const std::vector<BrowseDescription> browse_inputs{{.node_id = NodeId{1}}};
  adapter.Browse(
      context, browse_inputs,
      [browse_promise](Status status, std::vector<BrowseResult> results) mutable {
        EXPECT_TRUE(status);
        browse_promise.resolve(std::move(results));
      });
  auto browse_results = WaitForPromise(executor, browse_promise);
  ASSERT_EQ(browse_results.size(), 1u);
  EXPECT_EQ(browse_results[0].references[0].node_id, NodeId{3});
  EXPECT_EQ(service.last_browse_context.request_id(), context.request_id());
  EXPECT_EQ(service.last_browse_inputs, browse_inputs);

  promise<std::vector<BrowsePathResult>> translate_promise;
  const std::vector<BrowsePath> translate_inputs{
      {.node_id = NodeId{4}, .relative_path = {}}};
  adapter.TranslateBrowsePaths(
      translate_inputs,
      [translate_promise](Status status,
                          std::vector<BrowsePathResult> results) mutable {
        EXPECT_TRUE(status);
        translate_promise.resolve(std::move(results));
      });
  auto translate_results = WaitForPromise(executor, translate_promise);
  ASSERT_EQ(translate_results.size(), 1u);
  EXPECT_EQ(translate_results[0].targets[0].target_id.node_id(), NodeId{4});
  EXPECT_EQ(service.last_translate_inputs, translate_inputs);
}

TEST(CoroutineToCallbackNodeManagementServiceAdapter,
     ForwardsNodeManagementOperations) {
  auto executor = std::make_shared<TestExecutor>();
  TestCoroutineNodeManagementService service;
  CoroutineToCallbackNodeManagementServiceAdapter adapter{executor, service};

  promise<std::vector<AddNodesResult>> add_nodes_promise;
  const std::vector<AddNodesItem> add_nodes_inputs{
      {.requested_id = NodeId{1}, .parent_id = NodeId{2}}};
  adapter.AddNodes(
      add_nodes_inputs,
      [add_nodes_promise](Status status, std::vector<AddNodesResult> results) mutable {
        EXPECT_TRUE(status);
        add_nodes_promise.resolve(std::move(results));
      });
  EXPECT_EQ(WaitForPromise(executor, add_nodes_promise)[0].added_node_id,
            NodeId{5});
  EXPECT_EQ(service.last_add_nodes_inputs, add_nodes_inputs);

  promise<std::vector<StatusCode>> delete_nodes_promise;
  const std::vector<DeleteNodesItem> delete_nodes_inputs{
      {.node_id = NodeId{6}, .delete_target_references = true}};
  adapter.DeleteNodes(
      delete_nodes_inputs,
      [delete_nodes_promise](Status status, std::vector<StatusCode> results) mutable {
        EXPECT_TRUE(status);
        delete_nodes_promise.resolve(std::move(results));
      });
  EXPECT_THAT(WaitForPromise(executor, delete_nodes_promise),
              testing::ElementsAre(StatusCode::Good));
  ASSERT_EQ(service.last_delete_nodes_inputs.size(), delete_nodes_inputs.size());
  EXPECT_EQ(service.last_delete_nodes_inputs[0].node_id,
            delete_nodes_inputs[0].node_id);
  EXPECT_EQ(service.last_delete_nodes_inputs[0].delete_target_references,
            delete_nodes_inputs[0].delete_target_references);

  promise<std::vector<StatusCode>> add_references_promise;
  const std::vector<AddReferencesItem> add_references_inputs{
      {.source_node_id = NodeId{7},
       .reference_type_id = NodeId{8},
       .target_node_id = ExpandedNodeId{NodeId{9}}}};
  adapter.AddReferences(
      add_references_inputs,
      [add_references_promise](Status&& status,
                               std::vector<StatusCode>&& results) mutable {
        EXPECT_TRUE(status);
        add_references_promise.resolve(std::move(results));
      });
  EXPECT_THAT(WaitForPromise(executor, add_references_promise),
              testing::ElementsAre(StatusCode::Good));
  ASSERT_EQ(service.last_add_references_inputs.size(), add_references_inputs.size());
  EXPECT_EQ(service.last_add_references_inputs[0].source_node_id,
            add_references_inputs[0].source_node_id);
  EXPECT_EQ(service.last_add_references_inputs[0].reference_type_id,
            add_references_inputs[0].reference_type_id);
  EXPECT_EQ(service.last_add_references_inputs[0].target_node_id,
            add_references_inputs[0].target_node_id);

  promise<std::vector<StatusCode>> delete_references_promise;
  const std::vector<DeleteReferencesItem> delete_references_inputs{
      {.source_node_id = NodeId{10},
       .reference_type_id = NodeId{11},
       .target_node_id = ExpandedNodeId{NodeId{12}}}};
  adapter.DeleteReferences(
      delete_references_inputs,
      [delete_references_promise](Status&& status,
                                  std::vector<StatusCode>&& results) mutable {
        EXPECT_TRUE(status);
        delete_references_promise.resolve(std::move(results));
      });
  EXPECT_THAT(WaitForPromise(executor, delete_references_promise),
              testing::ElementsAre(StatusCode::Good));
  ASSERT_EQ(service.last_delete_references_inputs.size(),
            delete_references_inputs.size());
  EXPECT_EQ(service.last_delete_references_inputs[0].source_node_id,
            delete_references_inputs[0].source_node_id);
  EXPECT_EQ(service.last_delete_references_inputs[0].reference_type_id,
            delete_references_inputs[0].reference_type_id);
  EXPECT_EQ(service.last_delete_references_inputs[0].target_node_id,
            delete_references_inputs[0].target_node_id);
}

TEST(CoroutineToPromiseSessionServiceAdapter, ConnectsAndPassesThroughState) {
  auto executor = std::make_shared<TestExecutor>();
  TestCoroutineSessionService service;
  CoroutineToPromiseSessionServiceAdapter adapter{executor, service};

  SessionConnectParams params{.host = "opc.tcp://127.0.0.1:4840"};
  auto connect_promise = adapter.Connect(params);
  WaitForPromise(executor, connect_promise);
  EXPECT_EQ(service.last_connect_params.host, params.host);
  EXPECT_TRUE(service.connected);

  EXPECT_EQ(adapter.GetUserId(), service.user_id);
  EXPECT_TRUE(adapter.HasPrivilege(service.allowed_privilege));
  EXPECT_EQ(adapter.GetHostName(), service.host_name);
  EXPECT_TRUE(adapter.IsScada());

  auto disconnect_promise = adapter.Disconnect();
  WaitForPromise(executor, disconnect_promise);
  EXPECT_TRUE(service.disconnect_called);
  EXPECT_FALSE(service.connected);
}

}  // namespace
}  // namespace scada
