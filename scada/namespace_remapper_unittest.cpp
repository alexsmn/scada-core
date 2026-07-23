#include "scada/namespace_remapper.h"

#include "scada/event.h"

#include <gtest/gtest.h>

#include <optional>
#include <set>
#include <string>
#include <string_view>
#include <vector>

namespace scada::aggregation {
namespace {

const std::string kUa{kOpcUaNamespaceUri};

TEST(ProxyNamespaceTableTest, SeedsOpcUaAtIndexZero) {
  ProxyNamespaceTable table;
  ASSERT_EQ(table.uris().size(), 1u);
  EXPECT_EQ(table.uris()[0], kUa);
  EXPECT_EQ(table.Find(kUa), std::optional<scada::NamespaceIndex>{0});
  EXPECT_EQ(table.Find("urn:absent"), std::nullopt);
}

TEST(ProxyNamespaceTableTest, GetOrAddDedupesAndAppends) {
  ProxyNamespaceTable table;
  EXPECT_EQ(table.GetOrAdd(kUa), 0u);  // shared standard namespace
  EXPECT_EQ(table.GetOrAdd("urn:a"), 1u);
  EXPECT_EQ(table.GetOrAdd("urn:b"), 2u);
  EXPECT_EQ(table.GetOrAdd("urn:a"), 1u);  // de-duplicated
  EXPECT_EQ(table.uris().size(), 3u);
}

// ProxyNamespaceTable::GetOrAdd merges by URI equality, so any two canonical
// slots sharing a URI (in particular the historical "" placeholders) would
// collapse onto one proxy index when an Aggregating Server merges a
// downstream's NamespaceArray, silently mis-remapping every NodeId in the
// collapsed namespaces. OPC UA Part 3 §8.2.3 requires a URI per entry.
// https://reference.opcfoundation.org/Core/Part3/v105/docs/8.2.3
TEST(NamespaceRemapperTest, EmptyDownstreamUrisStayIdentityMappedAndUnmerged) {
  ProxyNamespaceTable table;
  const std::vector<std::string> downstream{kUa, "", "urn:server", ""};
  const NamespaceRemapper remapper =
      NamespaceRemapper::Build(downstream, table);

  // Empty slots are identity-mapped, not collapsed onto a shared index.
  EXPECT_EQ(remapper.ToProxy(scada::NodeId{1, 1}).namespace_index(), 1u);
  EXPECT_EQ(remapper.ToProxy(scada::NodeId{1, 3}).namespace_index(), 3u);
  // The real URI still merges normally.
  const scada::NamespaceIndex server_proxy =
      remapper.ToProxy(scada::NodeId{1, 2}).namespace_index();
  EXPECT_EQ(table.Find("urn:server"),
            std::optional<scada::NamespaceIndex>{server_proxy});
  // No empty entry was added to the proxy's served NamespaceArray.
  EXPECT_EQ(table.Find(""), std::nullopt);
  EXPECT_EQ(table.uris().size(), 2u);  // UA + urn:server
}

TEST(NamespaceRemapperTest, MergesSharedNamespacesAcrossServers) {
  ProxyNamespaceTable table;
  const std::vector<std::string> a{kUa, "urn:serverA", "urn:shared"};
  const std::vector<std::string> b{kUa, "urn:serverB", "urn:shared"};
  const NamespaceRemapper ra = NamespaceRemapper::Build(a, table);
  const NamespaceRemapper rb = NamespaceRemapper::Build(b, table);

  // The OPC UA namespace (downstream index 0) merges to proxy index 0 for both.
  EXPECT_EQ(ra.ToProxy(scada::NodeId{1, 0}).namespace_index(), 0u);
  EXPECT_EQ(rb.ToProxy(scada::NodeId{1, 0}).namespace_index(), 0u);

  // Each server's own URI (downstream index 1) gets a distinct proxy index.
  const scada::NamespaceIndex proxy_a1 =
      ra.ToProxy(scada::NodeId{1, 1}).namespace_index();
  const scada::NamespaceIndex proxy_b1 =
      rb.ToProxy(scada::NodeId{1, 1}).namespace_index();
  EXPECT_NE(proxy_a1, proxy_b1);

  // "urn:shared" (downstream index 2 on both) merges to a single proxy index.
  EXPECT_EQ(ra.ToProxy(scada::NodeId{1, 2}).namespace_index(),
            rb.ToProxy(scada::NodeId{1, 2}).namespace_index());

  // The proxy NamespaceArray holds: UA, serverA, shared, serverB.
  EXPECT_EQ(table.uris().size(), 4u);
}

TEST(NamespaceRemapperTest, RoundTripsEveryOwnedNamespace) {
  ProxyNamespaceTable table;
  const std::vector<std::string> uris{kUa, "urn:server", "urn:custom"};
  const NamespaceRemapper remapper = NamespaceRemapper::Build(uris, table);

  for (const scada::NamespaceIndex ns :
       std::vector<scada::NamespaceIndex>{0, 1, 2}) {
    const scada::NodeId downstream{42, ns};
    const scada::NodeId proxy = remapper.ToProxy(downstream);
    EXPECT_TRUE(remapper.OwnsProxyNamespace(proxy.namespace_index()));
    EXPECT_EQ(remapper.ToDownstream(proxy), downstream);
  }
}

TEST(NamespaceRemapperTest, RemapsStringNodeIdAndQualifiedName) {
  ProxyNamespaceTable table;
  // A first server takes proxy index 1 for its URI, so the second server's own
  // namespace (downstream index 1) maps to a DIFFERENT proxy index (2) — making
  // the translation observable rather than an identity.
  NamespaceRemapper::Build(std::vector<std::string>{kUa, "urn:other"}, table);
  const NamespaceRemapper remapper = NamespaceRemapper::Build(
      std::vector<std::string>{kUa, "urn:server"}, table);
  const scada::NamespaceIndex server_proxy = *table.Find("urn:server");
  ASSERT_NE(server_proxy, 1u);

  const scada::NodeId downstream{std::string{"item.1"}, 1};
  const scada::NodeId proxy = remapper.ToProxy(downstream);
  EXPECT_EQ(proxy.string_id(), "item.1");
  EXPECT_EQ(proxy.namespace_index(), server_proxy);  // 1 -> server_proxy
  EXPECT_EQ(remapper.ToDownstream(proxy), downstream);

  const scada::QualifiedName name{"BrowseName", 1};
  const scada::QualifiedName proxy_name = remapper.ToProxy(name);
  EXPECT_EQ(proxy_name.name(), "BrowseName");
  EXPECT_EQ(proxy_name.namespace_index(), server_proxy);
  EXPECT_EQ(remapper.ToDownstream(proxy_name).namespace_index(), 1u);
}

TEST(NamespaceRemapperTest, RemapsReferenceDescriptionFields) {
  ProxyNamespaceTable table;
  const std::vector<std::string> uris{kUa, "urn:server"};
  const NamespaceRemapper remapper = NamespaceRemapper::Build(uris, table);
  const scada::NamespaceIndex server_proxy =
      remapper.ToProxy(scada::NodeId{1, 1}).namespace_index();

  scada::ReferenceDescription reference;
  reference.reference_type_id = scada::NodeId{47, 0};  // HasComponent (ns 0)
  reference.node_id = scada::NodeId{5, 1};
  reference.browse_name = scada::QualifiedName{"Child", 1};
  reference.type_definition = scada::NodeId{58, 0};  // BaseObjectType (ns 0)

  const scada::ReferenceDescription proxy = remapper.ToProxy(reference);
  EXPECT_EQ(proxy.reference_type_id.namespace_index(), 0u);  // ns 0 stays ns 0
  EXPECT_EQ(proxy.node_id.namespace_index(), server_proxy);
  EXPECT_EQ(proxy.browse_name.namespace_index(), server_proxy);
  EXPECT_EQ(proxy.type_definition.namespace_index(), 0u);
}

TEST(NamespaceRemapperTest, ExpandedNodeIdRemappedByIndexOnlyWithoutUri) {
  ProxyNamespaceTable table;
  // Make downstream index 1 map to a distinct proxy index (2) so the remap is
  // observable (the first server already claimed proxy index 1).
  NamespaceRemapper::Build(std::vector<std::string>{kUa, "urn:other"}, table);
  const NamespaceRemapper remapper = NamespaceRemapper::Build(
      std::vector<std::string>{kUa, "urn:server"}, table);
  const scada::NamespaceIndex server_proxy = *table.Find("urn:server");

  // No namespace URI: the inner NodeId's index is remapped (1 -> server_proxy).
  const scada::ExpandedNodeId no_uri{scada::NodeId{7, 1}};
  EXPECT_EQ(remapper.ToProxy(no_uri).node_id().namespace_index(), server_proxy);

  // A namespace URI is server-independent and authoritative, so the index is
  // left unchanged.
  const scada::ExpandedNodeId with_uri{scada::NodeId{7, 1},
                                       std::string{"urn:x"}, 0};
  EXPECT_EQ(remapper.ToProxy(with_uri).node_id().namespace_index(), 1u);
}

TEST(NamespaceRemapperTest, UnknownProxyNamespaceLeftUntranslated) {
  ProxyNamespaceTable table;
  const std::vector<std::string> uris{kUa, "urn:server"};
  const NamespaceRemapper remapper = NamespaceRemapper::Build(uris, table);

  // A proxy namespace this downstream does not own (e.g. another server's) is
  // not translated; a router uses OwnsProxyNamespace to avoid routing it here.
  const scada::NamespaceIndex foreign = 99;
  EXPECT_FALSE(remapper.OwnsProxyNamespace(foreign));
  EXPECT_EQ(remapper.ToDownstream(scada::NodeId{1, foreign}).namespace_index(),
            foreign);
}

TEST(NamespaceRemapperTest, RemapsIdentifierValuesInsideVariant) {
  ProxyNamespaceTable table;
  // First server claims proxy index 1, so this server's ns 1 maps to a distinct
  // proxy index — making the value remapping observable.
  NamespaceRemapper::Build(std::vector<std::string>{kUa, "urn:other"}, table);
  const NamespaceRemapper remapper = NamespaceRemapper::Build(
      std::vector<std::string>{kUa, "urn:server"}, table);
  const scada::NamespaceIndex proxy_ns = *table.Find("urn:server");
  ASSERT_NE(proxy_ns, 1u);

  // Scalar NodeId value, both directions.
  const scada::Variant proxy =
      remapper.ToProxy(scada::Variant{scada::NodeId{5, 1}});
  ASSERT_TRUE(proxy.get_if<scada::NodeId>());
  EXPECT_EQ(*proxy.get_if<scada::NodeId>(), scada::NodeId(5, proxy_ns));
  EXPECT_EQ(*remapper.ToDownstream(proxy).get_if<scada::NodeId>(),
            scada::NodeId(5, 1));

  // QualifiedName value.
  const scada::Variant name =
      remapper.ToProxy(scada::Variant{scada::QualifiedName{"N", 1}});
  ASSERT_TRUE(name.get_if<scada::QualifiedName>());
  EXPECT_EQ(name.get_if<scada::QualifiedName>()->namespace_index(), proxy_ns);

  // Array of NodeIds: each element remapped, ns 0 left alone.
  const scada::Variant array = remapper.ToProxy(
      scada::Variant{std::vector<scada::NodeId>{{6, 1}, {7, 0}}});
  const auto* ids = array.get_if<std::vector<scada::NodeId>>();
  ASSERT_TRUE(ids);
  ASSERT_EQ(ids->size(), 2u);
  EXPECT_EQ((*ids)[0], scada::NodeId(6, proxy_ns));
  EXPECT_EQ((*ids)[1], scada::NodeId(7, 0));

  // A non-identifier value passes through unchanged.
  const scada::Variant passthrough =
      remapper.ToProxy(scada::Variant{scada::String{"plain"}});
  ASSERT_TRUE(passthrough.get_if<scada::String>());
  EXPECT_EQ(*passthrough.get_if<scada::String>(), "plain");
}

// Event payloads cross the aggregation boundary as typed structs whose NodeId
// fields carry downstream indexes (ADR 0003's untranslated-payload hole); the
// event overloads remap every namespace-sensitive field and leave everything
// else (id, times, message, ack state) untouched.
TEST(NamespaceRemapperTest, RemapsEventPayloadFields) {
  ProxyNamespaceTable table;
  NamespaceRemapper::Build(std::vector<std::string>{kUa, "urn:other"}, table);
  const NamespaceRemapper remapper = NamespaceRemapper::Build(
      std::vector<std::string>{kUa, "urn:server"}, table);
  const scada::NamespaceIndex proxy_ns =
      remapper.ToProxy(scada::NodeId{1, 1}).namespace_index();
  ASSERT_NE(proxy_ns, 1u);

  scada::Event event;
  event.event_type_id = scada::NodeId{100, 1};
  event.event_id = 42;
  event.time = scada::base::NowUtc();
  event.receive_time = scada::base::NowUtc();
  event.source_node_id = scada::NodeId{5, 1};
  event.user_id = scada::NodeId{6, 1};
  event.value = scada::Variant{scada::NodeId{7, 1}};
  event.message = u"message";
  event.acked = true;
  event.acknowledged_time = event.time;
  event.acknowledged_user_id = scada::NodeId{8, 1};

  const scada::Event remapped = remapper.ToProxy(event);
  EXPECT_EQ(remapped.event_type_id, scada::NodeId(100, proxy_ns));
  EXPECT_EQ(remapped.source_node_id, scada::NodeId(5, proxy_ns));
  EXPECT_EQ(remapped.user_id, scada::NodeId(6, proxy_ns));
  EXPECT_EQ(remapped.acknowledged_user_id, scada::NodeId(8, proxy_ns));
  EXPECT_EQ(*remapped.value.get_if<scada::NodeId>(),
            scada::NodeId(7, proxy_ns));
  // Identity and payload survive untouched.
  EXPECT_EQ(remapped.event_id, event.event_id);
  EXPECT_EQ(remapped.time, event.time);
  EXPECT_EQ(remapped.receive_time, event.receive_time);
  EXPECT_EQ(remapped.message, event.message);
  EXPECT_EQ(remapped.acked, event.acked);
  EXPECT_EQ(remapped.acknowledged_time, event.acknowledged_time);

  scada::ModelChangeEvent model_change;
  model_change.node_id = scada::NodeId{9, 1};
  model_change.type_definition_id = scada::NodeId{10, 1};
  model_change.verb = scada::ModelChangeEvent::NodeAdded;
  const scada::ModelChangeEvent remapped_model_change =
      remapper.ToProxy(model_change);
  EXPECT_EQ(remapped_model_change.node_id, scada::NodeId(9, proxy_ns));
  EXPECT_EQ(remapped_model_change.type_definition_id,
            scada::NodeId(10, proxy_ns));
  EXPECT_EQ(remapped_model_change.verb, model_change.verb);

  scada::SemanticChangeEvent semantic_change;
  semantic_change.node_id = scada::NodeId{11, 1};
  EXPECT_EQ(remapper.ToProxy(semantic_change).node_id,
            scada::NodeId(11, proxy_ns));
}

}  // namespace
}  // namespace scada::aggregation
