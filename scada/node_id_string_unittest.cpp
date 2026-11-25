#include "scada/node_id_string.h"

#include "scada/guid.h"

#include <gtest/gtest.h>

#include <optional>
#include <string>
#include <string_view>

namespace scada {
namespace {

// One text both written and read: the identifier and URI it stands for.
struct Vector {
  std::string_view text;
  std::string_view namespace_uri;  // empty: no URI in the text
  NodeId node_id;                  // namespace index 0 for the URI forms
};

ByteString Bytes(std::string_view text) {
  return ByteString{text.begin(), text.end()};
}

// The canonical forms: FormatNodeIdString writes exactly `text`, and
// ParseNodeIdString reads it back. Three come from the examples in OPC UA
// Part 6 §5.1.12 QualifiedName, NodeId and ExpandedNodeId String Encoding,
// https://reference.opcfoundation.org/Core/Part6/v105/docs/5.1.12 .
std::vector<Vector> CanonicalVectors() {
  return {
      {"i=13", "", NodeId{13u, 0}},
      {"i=2258", "", NodeId{2258u, 0}},
      {"nsu=urn:open62541.server.application;s=Plant.Temperature",
       "urn:open62541.server.application",
       NodeId{String{"Plant.Temperature"}, 0}},
      {"nsu=http://widgets.com/schemas/hello;s=水 World",
       "http://widgets.com/schemas/hello", NodeId{String{"水 World"}, 0}},
      {"nsu=tag:acme.com,2023:schemas:data#off%3B;b=M/RbKBsRVkePCePcx24oRA==",
       "tag:acme.com,2023:schemas:data#off;",
       NodeId{Bytes("\x33\xF4\x5B\x28\x1B\x11\x56\x47\x8F\x09\xE3\xDC\xC7\x6E"
                    "\x28\x44"),
              0}},
      {"nsu=http://telecontrol.ru/opcua/data_items/DiscreteItemType;i=1379",
       "http://telecontrol.ru/opcua/data_items/DiscreteItemType",
       NodeId{1379u, 0}},
      {"nsu=urn:x;g=09087E75-8E5E-499B-954F-F2A9603DB28A", "urn:x",
       NodeId{*Guid::FromString("09087e75-8e5e-499b-954f-f2a9603db28a"), 0}},
      // A namespace "URI" that is not one: every byte outside RFC 3986 is
      // escaped, '%' and '}' included, so a braced formula never ends early.
      {"nsu=my%20plant%7Bv%25%7D;s=a", "my plant{v%}", NodeId{String{"a"}, 0}},
      {"nsu=urn:x;s=", "urn:x", NodeId{String{}, 0}},
  };
}

TEST(NodeIdStringTest, FormatWritesTheCanonicalForm) {
  for (const Vector& vector : CanonicalVectors()) {
    SCOPED_TRACE(vector.text);
    EXPECT_EQ(FormatNodeIdString(vector.node_id, vector.namespace_uri),
              vector.text);
  }
}

TEST(NodeIdStringTest, ParseReadsTheCanonicalForm) {
  for (const Vector& vector : CanonicalVectors()) {
    SCOPED_TRACE(vector.text);
    const std::optional<ParsedNodeIdString> parsed =
        ParseNodeIdString(vector.text);
    ASSERT_TRUE(parsed.has_value());
    EXPECT_EQ(parsed->node_id, vector.node_id);
    if (vector.namespace_uri.empty()) {
      EXPECT_FALSE(parsed->namespace_uri.has_value());
    } else {
      EXPECT_EQ(parsed->namespace_uri, std::string{vector.namespace_uri});
    }
  }
}

TEST(NodeIdStringTest, FormatIgnoresTheNodeIdsOwnIndex) {
  // The URI names the namespace; a session's index for it is never written.
  EXPECT_EQ(FormatNodeIdString(NodeId{7u, 38}, "urn:x"), "nsu=urn:x;i=7");
}

TEST(NodeIdStringTest, TheOpcUaNamespaceIsWrittenBare) {
  // Part 6 §5.1.12: namespace 0 uses only the <identifier> form.
  EXPECT_EQ(
      FormatNodeIdString(NodeId{2258u, 0}, "http://opcfoundation.org/UA/"),
      "i=2258");
  EXPECT_EQ(FormatNodeIdString(NodeId{2258u, 5}, ""), "i=2258");
}

TEST(NodeIdStringTest, ReadsTheOpcUaNamespaceUriAsTheBareForm) {
  const auto parsed =
      ParseNodeIdString("nsu=http://opcfoundation.org/UA/;i=2258");
  ASSERT_TRUE(parsed.has_value());
  EXPECT_FALSE(parsed->namespace_uri.has_value());
  EXPECT_EQ(parsed->node_id, NodeId(2258u, 0));
}

TEST(NodeIdStringTest, ReadsTheIndexForm) {
  const auto parsed = ParseNodeIdString("ns=10;i=12345");
  ASSERT_TRUE(parsed.has_value());
  EXPECT_FALSE(parsed->namespace_uri.has_value());
  EXPECT_EQ(parsed->node_id, NodeId(12345u, 10));
}

TEST(NodeIdStringTest, PrefixesMatchInAnyCase) {
  // ABNF quoted strings are case-insensitive (RFC 5234 §2.3).
  const auto parsed = ParseNodeIdString("NSU=urn:x;I=5");
  ASSERT_TRUE(parsed.has_value());
  EXPECT_EQ(parsed->namespace_uri, std::string{"urn:x"});
  EXPECT_EQ(parsed->node_id, NodeId(5u, 0));
}

TEST(NodeIdStringTest, AStringIdentifierKeepsSemicolonsAndEquals) {
  // Only the first ';' ends the namespace part; the rest is the identifier.
  const auto parsed = ParseNodeIdString("nsu=urn:x;s=a;b=c");
  ASSERT_TRUE(parsed.has_value());
  EXPECT_EQ(parsed->node_id, NodeId(String{"a;b=c"}, 0));
}

TEST(NodeIdStringTest, RejectsWhatIsNotExactlyOneForm) {
  for (const std::string_view text : {
           "",
           "Plant.Temperature",  // NodeId::FromString would take this
           "TS.1379",
           "i=",
           "i=12x",
           "i=-1",
           "i=4294967296",
           "ns=65536;i=1",
           "ns=;i=1",
           "ns=1",
           "ns=1;Plant",
           "nsu=urn:x",
           "nsu=;i=1",
           "nsu=urn%2;i=1",
           "nsu=urn%zz;i=1",
           "g=not-a-guid",
           "b=AB$CD",
           "x=1",
       }) {
    SCOPED_TRACE(text);
    EXPECT_FALSE(ParseNodeIdString(text).has_value());
  }
}

TEST(NodeIdStringTest, EncodingRoundTripsEveryByte) {
  std::string all;
  for (int c = 0; c < 256; ++c)
    all += static_cast<char>(c);
  const std::string encoded = EncodeNamespaceUri(all);
  EXPECT_EQ(encoded.find(';'), std::string::npos);
  EXPECT_EQ(encoded.find('}'), std::string::npos);
  EXPECT_EQ(DecodeNamespaceUri(encoded), all);
}

TEST(NodeIdStringTest, AnAlreadyEscapedUriKeepsItsEscape) {
  // '%' is encoded too, so "a%20b" is read back as "a%20b", not "a b".
  EXPECT_EQ(EncodeNamespaceUri("a%20b"), "a%2520b");
  EXPECT_EQ(DecodeNamespaceUri("a%2520b"), std::string{"a%20b"});
}

TEST(NodeIdStringTest, DecodeAcceptsLowerCaseHex) {
  EXPECT_EQ(DecodeNamespaceUri("off%3b"), std::string{"off;"});
}

}  // namespace
}  // namespace scada
