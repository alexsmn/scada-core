#include "scada/node_id.h"

#include <gmock/gmock.h>

using namespace testing;

TEST(NodeId, Construct_Default_Null) {
  EXPECT_TRUE(scada::NodeId().is_null());
}

TEST(NodeId, Equality_StringId) {
  EXPECT_TRUE(scada::NodeId("string_id", 123) ==
              scada::NodeId("string_id", 123));
  EXPECT_FALSE(scada::NodeId("abc", 123) == scada::NodeId("def", 123));
  EXPECT_FALSE(scada::NodeId("string_id", 123) ==
               scada::NodeId("string_id", 456));
}

TEST(NodeId, Less_StringId) {
  EXPECT_FALSE(scada::NodeId("aaa", 123) < scada::NodeId("aaa", 123));
  EXPECT_TRUE(scada::NodeId("aaa", 123) < scada::NodeId("aaa", 456));
  EXPECT_FALSE(scada::NodeId("aaa", 456) < scada::NodeId("aaa", 123));

  EXPECT_TRUE(scada::NodeId("aaa", 123) < scada::NodeId("bbb", 123));
  EXPECT_FALSE(scada::NodeId("bbb", 123) < scada::NodeId("aaa", 123));
}

TEST(NodeId, Move) {
  auto a = scada::NodeId("string_id", 123);
  auto b = std::move(a);

  EXPECT_TRUE(a.is_null());
  EXPECT_EQ(b, scada::NodeId("string_id", 123));
}