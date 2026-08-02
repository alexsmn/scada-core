#include "scada/node_id_log.h"

#include <string_view>

#include <gtest/gtest.h>

namespace scada {
namespace {

std::string_view TestResolver(NamespaceIndex index) {
  switch (index) {
    case 7:
      return "SCADA";
    case 5:
      return "USER";
    default:
      return {};
  }
}

// Restores the process-wide resolver to "none" after each test so ordering does
// not leak state between cases.
class NodeIdLogTest : public testing::Test {
 protected:
  void TearDown() override { SetNamespaceNameResolver(nullptr); }
};

TEST_F(NodeIdLogTest, NumericFallbackWithoutResolver) {
  SetNamespaceNameResolver(nullptr);
  EXPECT_EQ(NodeIdToLogString(NodeId{123u, 7}), "NS7.123");
}

TEST_F(NodeIdLogTest, FriendlyNameWithResolver) {
  SetNamespaceNameResolver(&TestResolver);
  EXPECT_EQ(NodeIdToLogString(NodeId{123u, 7}), "SCADA.123");
  EXPECT_EQ(NodeIdToLogString(NodeId{5u, 5}), "USER.5");
  // Unknown namespace still falls back to the numeric prefix.
  EXPECT_EQ(NodeIdToLogString(NodeId{9u, 3}), "NS3.9");
}

TEST_F(NodeIdLogTest, StringIdentifier) {
  SetNamespaceNameResolver(&TestResolver);
  EXPECT_EQ(NodeIdToLogString(NodeId{String{"admin!x"}, 5}), "USER.admin!x");
}

}  // namespace
}  // namespace scada
