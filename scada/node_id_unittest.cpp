#include "scada/node_id.h"

#include "base/struct_format.h"

#include <format>
#include <sstream>

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

TEST(NodeId, ToString_OpaqueId) {
  scada::ByteString opaque_id{'a', 'b', 'c'};
  const scada::NodeId node_id{opaque_id, 0};

  EXPECT_EQ(node_id.ToString(), "b=YWJj");
}

TEST(NodeId, ToString_OpaqueIdWithNamespace) {
  scada::ByteString opaque_id{'\x01', '\x02', '\x03'};
  const scada::NodeId node_id{opaque_id, 2};

  EXPECT_EQ(node_id.ToString(), "ns=2;b=AQID");
}

TEST(NodeId, Format_MatchesOstream) {
  const scada::NodeId node_id{33, 2};

  std::ostringstream stream;
  stream << node_id;

  // The std::format-native formatter must produce the same quoted form as the
  // existing operator<<.
  EXPECT_EQ(std::format("{}", node_id), "\"ns=2;i=33\"");
  EXPECT_EQ(std::format("{}", node_id), stream.str());
}

TEST(NodeId, Format_RejectsFormatSpec) {
  scada::NodeId node_id{1};
  EXPECT_THROW(std::vformat("{:5}", std::make_format_args(node_id)),
               std::format_error);
}

TEST(NodeId, Format_EmbedsInStructFormatter) {
  // A NodeId can now be a field value in a std::format-native StructFormatter
  // without any std::ostream in the path — the shape a migrated composite type
  // (e.g. ModelChangeEvent) would use.
  const scada::NodeId node_id{7, 1};
  std::string out;
  StructFormatter{std::back_inserter(out)}
      .AddField("node_id", node_id)
      .Finish();

  EXPECT_EQ(out, "{node_id: \"ns=1;i=7\"}");
}
