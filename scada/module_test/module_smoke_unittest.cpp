// Smoke test for the scada.core module facade: names come from
// `import scada.core;` only, including scada.base / scada.metrics names
// re-exported through `export import`.

#include <format>
#include <string>
#include <unordered_map>

#include <gtest/gtest.h>

// Import after the textual includes (AppleClang 21 libc++ merging bug).
import scada.core;

namespace scada_core_module {
namespace {

TEST(ScadaCoreModuleSmoke, NodeIdBasics) {
  scada::NodeId node_id{42, 7};
  EXPECT_EQ(node_id.numeric_id(), 42u);
  EXPECT_EQ(node_id.namespace_index(), 7);

  // Namespace-scope free operators from node_id.h (exported explicitly).
  EXPECT_TRUE(node_id == scada::NumericId{42} ||
              node_id.namespace_index() != 0);
  EXPECT_TRUE(scada::NodeId(42, 0) == scada::NumericId{42});
  EXPECT_TRUE(scada::NodeId(43, 0) != scada::NumericId{42});
}

TEST(ScadaCoreModuleSmoke, StdSpecializationsReachable) {
  // std::hash<scada::NodeId> and std::formatter<scada::NodeId> live in the
  // GMF and are kept alive by static_asserts in the facade; these
  // instantiations fail to compile if a toolchain discards them.
  std::unordered_map<scada::NodeId, int> map;
  map[scada::NodeId{1, 2}] = 3;
  EXPECT_EQ(map.size(), 1u);
  EXPECT_FALSE(std::format("{}", scada::NodeId{1, 2}).empty());
}

TEST(ScadaCoreModuleSmoke, StatusAndVariant) {
  scada::Status status{scada::StatusCode::Good};
  EXPECT_TRUE(status);
  scada::StatusOr<int> ok{42};
  ASSERT_TRUE(ok.ok());
  EXPECT_EQ(*ok, 42);

  scada::Variant variant{scada::Int32{7}};
  EXPECT_FALSE(variant.is_null());
  scada::DataValue value{variant, scada::Qualifier{}, scada::Time{},
                         scada::Time{}};
  EXPECT_FALSE(value.value.is_null());

  // Global ToString overload set (exported once, spans several headers).
  EXPECT_FALSE(ToString(scada::StatusCode::Bad).empty());
  EXPECT_FALSE(ToString(scada::NodeClass::Variable).empty());
}

TEST(ScadaCoreModuleSmoke, ServicesAggregateAndTransitiveBase) {
  scada::services services{};
  EXPECT_EQ(services.attribute_service, nullptr);

  // Transitive scada.base and scada.metrics surfaces via export import.
  scada::base::Check(true, "core module smoke");
  EXPECT_EQ(Format(42), "42");
  EXPECT_EQ(scada::metrics::NormalizeGrpcEndpoint("https://h:1"), "h:1");
}

}  // namespace
}  // namespace scada_core_module
