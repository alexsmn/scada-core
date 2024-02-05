#include "scada/node_id.h"

#include <gmock/gmock.h>

using namespace testing;

TEST(NodeId, Equality_StringId) {
  EXPECT_EQ(scada::NodeId("string_id", 123), scada::NodeId("string_id", 123));
  EXPECT_NE(scada::NodeId("abc", 123), scada::NodeId("def", 123));
  EXPECT_NE(scada::NodeId("string_id", 123), scada::NodeId("string_id", 456));
}