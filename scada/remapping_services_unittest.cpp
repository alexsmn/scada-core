#include "scada/remapping_services.h"

#include "base/test/awaitable_test.h"
#include "base/test/test_executor.h"

#include "scada/co_result.h"
#include "scada/expanded_node_id.h"
#include "scada/service_context.h"
#include "scada/standard_node_ids.h"

#include <gtest/gtest.h>

#include <memory>
#include <span>
#include <string>
#include <vector>

namespace scada::aggregation {
namespace {

// Records the (already proxy->downstream translated) request and returns a
// canned response in the downstream namespace.
class FakeViewService : public scada::ViewService {
 public:
  std::vector<scada::BrowseDescription> recorded_browse;
  std::vector<scada::BrowseResult> browse_response;
  std::vector<scada::BrowsePath> recorded_paths;
  std::vector<scada::BrowsePathResult> translate_response;

  scada::CoStatusOr<std::vector<scada::BrowseResult>> Browse(
      scada::ServiceContext,
      std::vector<scada::BrowseDescription> inputs) override {
    recorded_browse = inputs;
    co_return browse_response;
  }

  scada::CoStatusOr<std::vector<scada::BrowsePathResult>> TranslateBrowsePaths(
      std::vector<scada::BrowsePath> inputs) override {
    recorded_paths = inputs;
    co_return translate_response;
  }
};

class FakeAttributeService : public scada::AttributeService {
 public:
  std::vector<scada::ReadValueId> recorded_read;
  std::vector<scada::DataValue> read_response;
  std::vector<scada::WriteValue> recorded_write;
  std::vector<scada::StatusCode> write_response;

  scada::CoStatusOr<std::vector<scada::DataValue>> Read(
      scada::ServiceContext,
      std::vector<scada::ReadValueId> inputs) override {
    recorded_read = std::move(inputs);
    co_return read_response;
  }

  scada::CoStatusOr<std::vector<scada::StatusCode>> Write(
      scada::ServiceContext,
      std::vector<scada::WriteValue> inputs) override {
    recorded_write = std::move(inputs);
    co_return write_response;
  }
};

class FakeMethodService : public scada::MethodService {
 public:
  scada::NodeId recorded_object;
  scada::NodeId recorded_method;
  std::vector<scada::Variant> recorded_arguments;

  std::vector<scada::Variant> outputs;

  scada::CoStatusOr<scada::CallResult> Call(scada::NodeId node_id,
                                            scada::NodeId method_id,
                                            std::vector<scada::Variant> arguments,
                                            scada::ServiceContext) override {
    recorded_object = node_id;
    recorded_method = method_id;
    recorded_arguments = std::move(arguments);
    co_return scada::CallResult{outputs};
  }
};

class FakeHistoryService : public scada::HistoryService {
 public:
  scada::HistoryReadRawDetails recorded_raw;
  std::vector<scada::DataValue> raw_response;
  scada::NodeId recorded_events_node;

  scada::CoStatusOr<scada::HistoryReadRawResult> HistoryReadRaw(
      scada::HistoryReadRawDetails details) override {
    recorded_raw = details;
    co_return scada::HistoryReadRawResult{.values = raw_response};
  }

  scada::CoStatusOr<scada::HistoryReadEventsResult> HistoryReadEvents(
      scada::NodeId node_id,
      scada::Time,
      scada::Time,
      scada::EventFilter) override {
    recorded_events_node = node_id;
    co_return scada::HistoryReadEventsResult{};
  }
};

class FakeNodeManagementService : public scada::NodeManagementService {
 public:
  std::vector<scada::AddNodesItem> recorded_add;
  std::vector<scada::AddNodesResult> add_response;
  std::vector<scada::DeleteNodesItem> recorded_delete;
  std::vector<scada::AddReferencesItem> recorded_add_references;

  scada::CoStatusOr<std::vector<scada::AddNodesResult>> AddNodes(
      scada::ServiceContext,
      std::vector<scada::AddNodesItem> inputs) override {
    recorded_add = std::move(inputs);
    co_return add_response;
  }

  scada::CoStatusOr<std::vector<scada::StatusCode>> DeleteNodes(
      scada::ServiceContext,
      std::vector<scada::DeleteNodesItem> inputs) override {
    recorded_delete = std::move(inputs);
    co_return std::vector<scada::StatusCode>(recorded_delete.size(),
                                             scada::StatusCode::Good);
  }

  scada::CoStatusOr<std::vector<scada::StatusCode>> AddReferences(
      scada::ServiceContext,
      std::vector<scada::AddReferencesItem> inputs) override {
    recorded_add_references = std::move(inputs);
    co_return std::vector<scada::StatusCode>(recorded_add_references.size(),
                                             scada::StatusCode::Good);
  }

  scada::CoStatusOr<std::vector<scada::StatusCode>> DeleteReferences(
      scada::ServiceContext,
      std::vector<scada::DeleteReferencesItem> inputs) override {
    co_return std::vector<scada::StatusCode>(inputs.size(),
                                             scada::StatusCode::Good);
  }
};

class FakeMonitoredItemSubscription : public scada::MonitoredItemSubscription {
 public:
  explicit FakeMonitoredItemSubscription(
      std::vector<scada::MonitoredItemCreateRequest>* recorded)
      : recorded_{recorded} {}

  Awaitable<std::vector<scada::MonitoredItemCreateResult>> AddItems(
      std::vector<scada::MonitoredItemCreateRequest> requests) override {
    *recorded_ = requests;
    co_return std::vector<scada::MonitoredItemCreateResult>(requests.size());
  }
  Awaitable<std::vector<scada::Status>> RemoveItems(
      std::span<const scada::MonitoredItemId>) override {
    co_return std::vector<scada::Status>{};
  }
  scada::CoStatusOr<std::vector<scada::MonitoredItemNotification>> ReadNext(
      std::size_t) override {
    co_return std::vector<scada::MonitoredItemNotification>{};
  }
  void Close(scada::Status) override {}

 private:
  std::vector<scada::MonitoredItemCreateRequest>* recorded_;
};

class FakeMonitoredItemService : public scada::MonitoredItemService {
 public:
  std::vector<scada::MonitoredItemCreateRequest> recorded_add;

  scada::StatusOr<std::unique_ptr<scada::MonitoredItemSubscription>>
  CreateSubscription(scada::ServiceContext,
                     scada::MonitoredItemSubscriptionOptions) override {
    std::unique_ptr<scada::MonitoredItemSubscription> subscription =
        std::make_unique<FakeMonitoredItemSubscription>(&recorded_add);
    return subscription;
  }
};

// Builds a remapper where the downstream's own namespace (downstream index 1)
// maps to a DIFFERENT proxy index, so the translation is observable. Returns
// the remapper plus the proxy index that downstream index 1 maps to.
struct Fixture {
  ProxyNamespaceTable table;
  NamespaceRemapper remapper;
  scada::NamespaceIndex downstream_ns = 1;
  scada::NamespaceIndex proxy_ns = 0;

  Fixture() {
    // A first server claims proxy index 1; this server's URI then gets index 2.
    NamespaceRemapper::Build(
        std::vector<std::string>{std::string{kOpcUaNamespaceUri}, "urn:other"},
        table);
    remapper = NamespaceRemapper::Build(
        std::vector<std::string>{std::string{kOpcUaNamespaceUri}, "urn:server"},
        table);
    proxy_ns = *table.Find("urn:server");
  }
};

TEST(RemappingViewServiceTest, RemapsBrowseRequestAndResponse) {
  Fixture fixture;
  FakeViewService inner;
  // Downstream returns a reference in its own namespace; ns 0 (HasComponent /
  // BaseObjectType) stays ns 0.
  scada::ReferenceDescription downstream_ref;
  downstream_ref.reference_type_id = scada::NodeId{47, 0};
  downstream_ref.node_id = scada::NodeId{5, fixture.downstream_ns};
  downstream_ref.browse_name =
      scada::QualifiedName{"Child", fixture.downstream_ns};
  downstream_ref.type_definition = scada::NodeId{58, 0};
  inner.browse_response = {scada::BrowseResult{.references = {downstream_ref}}};

  RemappingViewService service{inner, fixture.remapper};
  TestExecutor executor;
  auto result = WaitAwaitable(
      executor,
      service.Browse(scada::ServiceContext{},
                     {scada::BrowseDescription{
                         .node_id = scada::NodeId{3, fixture.proxy_ns},
                         .reference_type_id = scada::NodeId{35, 0}}}));

  // Request: proxy -> downstream.
  ASSERT_EQ(inner.recorded_browse.size(), 1u);
  EXPECT_EQ(inner.recorded_browse[0].node_id,
            scada::NodeId(3, fixture.downstream_ns));
  EXPECT_EQ(inner.recorded_browse[0].reference_type_id.namespace_index(), 0u);

  // Response: downstream -> proxy.
  ASSERT_TRUE(result.ok());
  ASSERT_EQ(result->size(), 1u);
  ASSERT_EQ((*result)[0].references.size(), 1u);
  const auto& ref = (*result)[0].references[0];
  EXPECT_EQ(ref.node_id, scada::NodeId(5, fixture.proxy_ns));
  EXPECT_EQ(ref.browse_name.namespace_index(), fixture.proxy_ns);
  EXPECT_EQ(ref.type_definition.namespace_index(), 0u);  // ns 0 unchanged
}

TEST(RemappingViewServiceTest, RemapsTranslateBrowsePathsRequestAndTargets) {
  Fixture fixture;
  FakeViewService inner;
  inner.translate_response = {scada::BrowsePathResult{
      .targets = {scada::BrowsePathTarget{
          .target_id = scada::ExpandedNodeId{
              scada::NodeId{9, fixture.downstream_ns}}}}}};

  RemappingViewService service{inner, fixture.remapper};
  TestExecutor executor;
  scada::BrowsePath path;
  path.node_id = scada::NodeId{3, fixture.proxy_ns};
  path.relative_path = {scada::RelativePathElement{
      .reference_type_id = scada::NodeId{47, 0},
      .target_name = scada::QualifiedName{"y", fixture.proxy_ns}}};
  auto result = WaitAwaitable(executor, service.TranslateBrowsePaths({path}));

  // Request: proxy -> downstream.
  ASSERT_EQ(inner.recorded_paths.size(), 1u);
  EXPECT_EQ(inner.recorded_paths[0].node_id,
            scada::NodeId(3, fixture.downstream_ns));
  ASSERT_EQ(inner.recorded_paths[0].relative_path.size(), 1u);
  EXPECT_EQ(
      inner.recorded_paths[0].relative_path[0].target_name.namespace_index(),
      fixture.downstream_ns);

  // Targets: downstream -> proxy.
  ASSERT_TRUE(result.ok());
  ASSERT_EQ((*result)[0].targets.size(), 1u);
  EXPECT_EQ((*result)[0].targets[0].target_id.node_id(),
            scada::NodeId(9, fixture.proxy_ns));
}

TEST(RemappingAttributeServiceTest, RemapsReadAndWriteTargets) {
  Fixture fixture;
  FakeAttributeService inner;

  RemappingAttributeService service{inner, fixture.remapper};
  TestExecutor executor;

  WaitAwaitable(
      executor,
      service.Read(scada::ServiceContext{},
                   std::vector<scada::ReadValueId>{
                       {.node_id = scada::NodeId{7, fixture.proxy_ns}}}));
  ASSERT_EQ(inner.recorded_read.size(), 1u);
  EXPECT_EQ(inner.recorded_read[0].node_id,
            scada::NodeId(7, fixture.downstream_ns));

  WaitAwaitable(
      executor,
      service.Write(scada::ServiceContext{},
                    std::vector<scada::WriteValue>{
                        {.node_id = scada::NodeId{8, fixture.proxy_ns}}}));
  ASSERT_EQ(inner.recorded_write.size(), 1u);
  EXPECT_EQ(inner.recorded_write[0].node_id,
            scada::NodeId(8, fixture.downstream_ns));
}

TEST(RemappingMethodServiceTest, RemapsObjectAndMethodIds) {
  Fixture fixture;
  FakeMethodService inner;
  RemappingMethodService service{inner, fixture.remapper};
  TestExecutor executor;

  auto status = WaitAwaitable(
      executor,
      service.Call(scada::NodeId{10, fixture.proxy_ns},
                   scada::NodeId{11, fixture.proxy_ns},
                   {scada::Variant{scada::NodeId{12, fixture.proxy_ns}}},
                   scada::ServiceContext{}));
  EXPECT_TRUE(status);
  EXPECT_EQ(inner.recorded_object, scada::NodeId(10, fixture.downstream_ns));
  EXPECT_EQ(inner.recorded_method, scada::NodeId(11, fixture.downstream_ns));
  // Identifier-typed argument values are remapped too.
  ASSERT_EQ(inner.recorded_arguments.size(), 1u);
  ASSERT_TRUE(inner.recorded_arguments[0].get_if<scada::NodeId>());
  EXPECT_EQ(*inner.recorded_arguments[0].get_if<scada::NodeId>(),
            scada::NodeId(12, fixture.downstream_ns));
}

// The mirror of the input remap above. A method's output arguments come back in
// the downstream's namespaces, so without the inverse mapping the proxy hands a
// client node ids from a namespace table the client never saw and cannot
// address.
TEST(RemappingMethodServiceTest, RemapsIdentifierValuesInOutputArguments) {
  Fixture fixture;
  FakeMethodService inner;
  inner.outputs = {scada::Variant{scada::NodeId{77, fixture.downstream_ns}},
                   scada::Variant{scada::Int32{5}}};
  RemappingMethodService service{inner, fixture.remapper};
  TestExecutor executor;

  auto result = WaitAwaitable(
      executor, service.Call(scada::NodeId{10, fixture.proxy_ns},
                             scada::NodeId{11, fixture.proxy_ns}, {},
                             scada::ServiceContext{}));

  ASSERT_TRUE(result.ok());
  ASSERT_EQ(result->output_arguments.size(), 2u);
  ASSERT_TRUE(result->output_arguments[0].get_if<scada::NodeId>());
  EXPECT_EQ(*result->output_arguments[0].get_if<scada::NodeId>(),
            scada::NodeId(77, fixture.proxy_ns));
  // Non-identifier outputs pass through untouched.
  EXPECT_EQ(result->output_arguments[1].get<scada::Int32>(), 5);
}

TEST(RemappingAttributeServiceTest, RemapsIdentifierValuesInReadResults) {
  Fixture fixture;
  FakeAttributeService inner;
  scada::DataValue data_value;
  data_value.value = scada::Variant{scada::NodeId{99, fixture.downstream_ns}};
  inner.read_response = {data_value};

  RemappingAttributeService service{inner, fixture.remapper};
  TestExecutor executor;
  auto read = WaitAwaitable(
      executor,
      service.Read(scada::ServiceContext{},
                   std::vector<scada::ReadValueId>{
                       {.node_id = scada::NodeId{1, fixture.proxy_ns}}}));
  ASSERT_TRUE(read.ok());
  ASSERT_EQ(read->size(), 1u);
  ASSERT_TRUE((*read)[0].value.get_if<scada::NodeId>());
  EXPECT_EQ(*(*read)[0].value.get_if<scada::NodeId>(),
            scada::NodeId(99, fixture.proxy_ns));
}

TEST(RemappingHistoryServiceTest, RemapsReadRawRequestAndIdentifierValues) {
  Fixture fixture;
  FakeHistoryService inner;
  scada::DataValue historical_value;
  historical_value.value =
      scada::Variant{scada::NodeId{99, fixture.downstream_ns}};
  inner.raw_response = {historical_value};

  RemappingHistoryService service{inner, fixture.remapper};
  TestExecutor executor;
  auto result = WaitAwaitable(
      executor, service.HistoryReadRaw(scada::HistoryReadRawDetails{
                    .node_id = scada::NodeId{7, fixture.proxy_ns}}));

  // Request: proxy -> downstream.
  EXPECT_EQ(inner.recorded_raw.node_id,
            scada::NodeId(7, fixture.downstream_ns));

  // Identifier-typed historical values: downstream -> proxy.
  ASSERT_TRUE(result.ok()) << result.status();
  ASSERT_EQ(result->values.size(), 1u);
  ASSERT_TRUE(result->values[0].value.get_if<scada::NodeId>());
  EXPECT_EQ(*result->values[0].value.get_if<scada::NodeId>(),
            scada::NodeId(99, fixture.proxy_ns));
}

TEST(RemappingHistoryServiceTest, RemapsReadEventsSourceNode) {
  Fixture fixture;
  FakeHistoryService inner;
  RemappingHistoryService service{inner, fixture.remapper};
  TestExecutor executor;

  WaitAwaitable(executor, service.HistoryReadEvents(
                              scada::NodeId{5, fixture.proxy_ns}, scada::Time{},
                              scada::Time{}, scada::EventFilter{}));
  EXPECT_EQ(inner.recorded_events_node,
            scada::NodeId(5, fixture.downstream_ns));
}

TEST(RemappingMonitoredItemServiceTest, RemapsAddedItemTargets) {
  Fixture fixture;
  FakeMonitoredItemService inner;
  RemappingMonitoredItemService service{inner, fixture.remapper};

  auto subscription = service.CreateSubscription(scada::ServiceContext{}, {});
  ASSERT_TRUE(subscription.ok());

  TestExecutor executor;
  scada::MonitoredItemCreateRequest request;
  request.item_to_monitor.node_id = scada::NodeId{12, fixture.proxy_ns};
  WaitAwaitable(executor, (*subscription)->AddItems({request}));

  ASSERT_EQ(inner.recorded_add.size(), 1u);
  EXPECT_EQ(inner.recorded_add[0].item_to_monitor.node_id,
            scada::NodeId(12, fixture.downstream_ns));
}

TEST(RemappingNodeManagementServiceTest, RemapsAddNodesRequestAndResultIds) {
  Fixture fixture;
  FakeNodeManagementService inner;
  // The downstream assigns the new node an id in its own namespace.
  inner.add_response = {scada::AddNodesResult{
      .status_code = scada::StatusCode::Good,
      .added_node_id = scada::NodeId{"f.txt", fixture.downstream_ns}}};

  RemappingNodeManagementService service{inner, fixture.remapper};
  TestExecutor executor;
  scada::AddNodesItem item;
  item.parent_id = scada::NodeId{304, fixture.proxy_ns};
  item.type_definition_id = scada::NodeId{306, fixture.proxy_ns};
  item.attributes.browse_name = scada::QualifiedName{"f.txt", fixture.proxy_ns};
  item.attributes.value = scada::Variant{scada::NodeId{12, fixture.proxy_ns}};
  auto result = WaitAwaitable(
      executor, service.AddNodes(scada::ServiceContext{}, {item}));

  // Request: proxy -> downstream, including identifier-typed attributes.
  ASSERT_EQ(inner.recorded_add.size(), 1u);
  EXPECT_EQ(inner.recorded_add[0].parent_id,
            scada::NodeId(304, fixture.downstream_ns));
  EXPECT_EQ(inner.recorded_add[0].type_definition_id,
            scada::NodeId(306, fixture.downstream_ns));
  EXPECT_EQ(inner.recorded_add[0].attributes.browse_name.namespace_index(),
            fixture.downstream_ns);
  ASSERT_TRUE(inner.recorded_add[0].attributes.value.has_value());
  EXPECT_EQ(*inner.recorded_add[0].attributes.value->get_if<scada::NodeId>(),
            scada::NodeId(12, fixture.downstream_ns));

  // Result ids: downstream -> proxy.
  ASSERT_TRUE(result.ok());
  ASSERT_EQ(result->size(), 1u);
  EXPECT_EQ((*result)[0].added_node_id,
            scada::NodeId("f.txt", fixture.proxy_ns));
}

TEST(RemappingNodeManagementServiceTest, RemapsDeleteAndReferenceTargets) {
  Fixture fixture;
  FakeNodeManagementService inner;
  RemappingNodeManagementService service{inner, fixture.remapper};
  TestExecutor executor;

  WaitAwaitable(executor,
                service.DeleteNodes(
                    scada::ServiceContext{},
                    {scada::DeleteNodesItem{
                        .node_id = scada::NodeId{"f.txt", fixture.proxy_ns}}}));
  ASSERT_EQ(inner.recorded_delete.size(), 1u);
  EXPECT_EQ(inner.recorded_delete[0].node_id,
            scada::NodeId("f.txt", fixture.downstream_ns));

  WaitAwaitable(executor,
                service.AddReferences(
                    scada::ServiceContext{},
                    {scada::AddReferencesItem{
                        .source_node_id = scada::NodeId{1, fixture.proxy_ns},
                        .reference_type_id = scada::NodeId{35, 0},
                        .target_node_id = scada::ExpandedNodeId{
                            scada::NodeId{2, fixture.proxy_ns}}}}));
  ASSERT_EQ(inner.recorded_add_references.size(), 1u);
  EXPECT_EQ(inner.recorded_add_references[0].source_node_id,
            scada::NodeId(1, fixture.downstream_ns));
  EXPECT_EQ(
      inner.recorded_add_references[0].reference_type_id.namespace_index(), 0u);
  EXPECT_EQ(inner.recorded_add_references[0].target_node_id.node_id(),
            scada::NodeId(2, fixture.downstream_ns));
}

TEST(MountViewServiceTest, KeepsOnlyDownstreamOwnNamespaceChildren) {
  Fixture fixture;
  FakeViewService inner;
  // The downstream's Objects-folder children, already namespace-remapped: a ns
  // 0 standard node (its Server) and a node in the downstream's own namespace.
  scada::ReferenceDescription server_ref;
  server_ref.node_id = scada::NodeId{2253, 0};
  scada::ReferenceDescription own_ref;
  own_ref.node_id = scada::NodeId{5, fixture.proxy_ns};
  inner.browse_response = {
      scada::BrowseResult{.references = {server_ref, own_ref}}};

  MountViewService service{inner};
  TestExecutor executor;
  auto result = WaitAwaitable(
      executor,
      service.Browse(scada::ServiceContext{},
                     {scada::BrowseDescription{
                         .node_id = scada::NodeId{scada::id::ObjectsFolder}}}));
  ASSERT_TRUE(result.ok());
  ASSERT_EQ(result->size(), 1u);
  ASSERT_EQ((*result)[0].references.size(), 1u);
  EXPECT_EQ((*result)[0].references[0].node_id,
            scada::NodeId(5, fixture.proxy_ns));
}

}  // namespace
}  // namespace scada::aggregation
