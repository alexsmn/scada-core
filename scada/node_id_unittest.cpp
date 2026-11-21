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
  EXPECT_THROW(
      static_cast<void>(std::vformat("{:5}", std::make_format_args(node_id))),
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

// `FromString` had no coverage at all until 486, which is how a verbatim
// duplicate of the `s=` branch sat below the live one — unreachable, and
// therefore invisible to every test and to the compiler. These pin the string
// forms so an edit landing on the wrong copy of a branch is caught.

TEST(NodeId, FromString_StringId) {
  const auto node_id = scada::NodeId::FromString("s=some_tag");

  EXPECT_EQ(node_id, scada::NodeId("some_tag", 0));
}

TEST(NodeId, FromString_StringIdWithNamespace) {
  const auto node_id = scada::NodeId::FromString("ns=2;s=some_tag");

  EXPECT_EQ(node_id, scada::NodeId("some_tag", 2));
}

TEST(NodeId, FromString_StringIdKeepsSeparatorsInTheIdentifier) {
  // The identifier runs to the end of the input: everything after the first
  // `s=` belongs to it, separators included.
  const auto node_id = scada::NodeId::FromString("ns=1;s=a;b=c");

  EXPECT_EQ(node_id, scada::NodeId("a;b=c", 1));
}

TEST(NodeId, FromString_NumericId) {
  EXPECT_EQ(scada::NodeId::FromString("i=33"), scada::NodeId(33u, 0));
  EXPECT_EQ(scada::NodeId::FromString("ns=2;i=33"), scada::NodeId(33u, 2));
}

TEST(NodeId, FromString_BareStringIsAStringId) {
  EXPECT_EQ(scada::NodeId::FromString("some_tag"),
            scada::NodeId("some_tag", 0));
}

TEST(NodeId, FromString_Empty_Null) {
  EXPECT_TRUE(scada::NodeId::FromString("").is_null());
}

TEST(NodeId, FromString_RoundTripsToString) {
  const scada::Guid guid =
      *scada::Guid::FromString("C496578A-0DFE-4B8F-870A-745238C6AEAE");
  for (const auto& node_id :
       {scada::NodeId{33u, 2}, scada::NodeId{"tag", 1}, scada::NodeId{"tag", 0},
        scada::NodeId{guid, 3}, scada::NodeId{guid, 0},
        scada::NodeId{scada::ByteString{'\x01', '\x00', '\xfe'}, 2}}) {
    EXPECT_EQ(scada::NodeId::FromString(node_id.ToString()), node_id);
  }
}

TEST(NodeId, GuidId) {
  const scada::Guid guid =
      *scada::Guid::FromString("C496578A-0DFE-4B8F-870A-745238C6AEAE");
  const scada::NodeId node_id{guid, 2};

  EXPECT_EQ(node_id.type(), scada::NodeIdType::Guid);
  EXPECT_TRUE(node_id.is_guid());
  EXPECT_FALSE(node_id.is_null());
  EXPECT_EQ(node_id.guid_id(), guid);
  EXPECT_EQ(node_id.ToString(), "ns=2;g=C496578A-0DFE-4B8F-870A-745238C6AEAE");
}

// A null Guid is still a Guid id, not the null NodeId: the tree treats only
// Numeric 0 in namespace 0 as null, for every form alike.
TEST(NodeId, NullGuidIsNotTheNullNodeId) {
  EXPECT_FALSE(scada::NodeId(scada::Guid{}, 0).is_null());
}

TEST(NodeId, GuidIdEqualityHashAndOrdering) {
  const scada::Guid a =
      *scada::Guid::FromString("C496578A-0DFE-4B8F-870A-745238C6AEAE");
  scada::Guid b = a;
  b.data4[7] = 0xAF;

  EXPECT_EQ(scada::NodeId(a, 1), scada::NodeId(a, 1));
  EXPECT_NE(scada::NodeId(a, 1), scada::NodeId(a, 2));
  EXPECT_NE(scada::NodeId(a, 1), scada::NodeId(b, 1));
  EXPECT_LT(scada::NodeId(a, 1), scada::NodeId(b, 1));
  EXPECT_EQ(std::hash<scada::NodeId>{}(scada::NodeId(a, 1)),
            std::hash<scada::NodeId>{}(scada::NodeId(a, 1)));

  // Guid sorts after every form that existed before it.
  EXPECT_LT(scada::NodeId(4000000000u, 9), scada::NodeId(a, 0));
  EXPECT_LT(scada::NodeId("zzz", 9), scada::NodeId(a, 0));
  EXPECT_LT(scada::NodeId(scada::ByteString{'\xff'}, 9), scada::NodeId(a, 0));
}

TEST(NodeId, FromString_GuidId) {
  const scada::Guid guid =
      *scada::Guid::FromString("C496578A-0DFE-4B8F-870A-745238C6AEAE");
  EXPECT_EQ(
      scada::NodeId::FromString("ns=4;g=c496578a-0dfe-4b8f-870a-745238c6aeae"),
      scada::NodeId(guid, 4));
}

TEST(NodeId, FromString_MalformedGuidIsNull) {
  EXPECT_TRUE(scada::NodeId::FromString("ns=4;g=not-a-guid").is_null());
}

// ToString has always written opaque ids as b=<base64>; FromString used to
// read that back as a String id "b=...".
TEST(NodeId, FromString_OpaqueId) {
  const scada::NodeId node_id = scada::NodeId::FromString("ns=2;b=AQL+");
  ASSERT_EQ(node_id.type(), scada::NodeIdType::Opaque);
  EXPECT_EQ(node_id,
            scada::NodeId(scada::ByteString{'\x01', '\x02', '\xfe'}, 2));
}

TEST(NodeId, MoveFromGuidLeavesNull) {
  scada::NodeId source{scada::Guid{.data1 = 1}, 2};
  const scada::NodeId target{std::move(source)};
  EXPECT_TRUE(target.is_guid());
  EXPECT_TRUE(source.is_null());  // NOLINT(bugprone-use-after-move)
}
