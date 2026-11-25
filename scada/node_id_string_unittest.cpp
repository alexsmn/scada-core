#include "scada/node_id_string.h"

#include "scada/guid.h"

#include <gtest/gtest.h>

#include <iterator>
#include <optional>
#include <string>
#include <string_view>

namespace scada {
namespace {

// One vector of node_id_string_vectors.inc, as the file spells it.
struct Vector {
  std::string_view text;
  std::string_view namespace_uri;  // empty: no URI in the text
  NamespaceIndex namespace_index = 0;
  char kind = 'i';  // 'i', 's', 'g' or 'b'
  std::string_view value;
};

#define NODE_ID_STRING_NONE(...)
#define NODE_ID_STRING_CANONICAL_ROW(text, uri, kind, value) \
  {text, uri, 0, kind, value},
#define NODE_ID_STRING_READ_ROW(text, uri, ns, kind, value) \
  {text, uri, ns, kind, value},
#define NODE_ID_STRING_REJECTED_ROW(text) {text},

// Written exactly as `text` and read back. Three come from the examples in
// OPC UA Part 6 §5.1.12 QualifiedName, NodeId and ExpandedNodeId String
// Encoding, https://reference.opcfoundation.org/Core/Part6/v105/docs/5.1.12 .
constexpr Vector kCanonical[] = {
#define NODE_ID_STRING_CANONICAL NODE_ID_STRING_CANONICAL_ROW
#define NODE_ID_STRING_WRITTEN NODE_ID_STRING_NONE
#define NODE_ID_STRING_READ NODE_ID_STRING_NONE
#define NODE_ID_STRING_REJECTED NODE_ID_STRING_NONE
#include "scada/node_id_string_vectors.inc"
#undef NODE_ID_STRING_CANONICAL
#undef NODE_ID_STRING_WRITTEN
#undef NODE_ID_STRING_READ
#undef NODE_ID_STRING_REJECTED
};

// Written as `text`, read back as another URI.
constexpr Vector kWritten[] = {
#define NODE_ID_STRING_CANONICAL NODE_ID_STRING_NONE
#define NODE_ID_STRING_WRITTEN NODE_ID_STRING_CANONICAL_ROW
#define NODE_ID_STRING_READ NODE_ID_STRING_NONE
#define NODE_ID_STRING_REJECTED NODE_ID_STRING_NONE
#include "scada/node_id_string_vectors.inc"
#undef NODE_ID_STRING_CANONICAL
#undef NODE_ID_STRING_WRITTEN
#undef NODE_ID_STRING_READ
#undef NODE_ID_STRING_REJECTED
};

// Read, but not what the writer writes.
constexpr Vector kRead[] = {
#define NODE_ID_STRING_CANONICAL NODE_ID_STRING_NONE
#define NODE_ID_STRING_WRITTEN NODE_ID_STRING_NONE
#define NODE_ID_STRING_READ NODE_ID_STRING_READ_ROW
#define NODE_ID_STRING_REJECTED NODE_ID_STRING_NONE
#include "scada/node_id_string_vectors.inc"
#undef NODE_ID_STRING_CANONICAL
#undef NODE_ID_STRING_WRITTEN
#undef NODE_ID_STRING_READ
#undef NODE_ID_STRING_REJECTED
};

// Not exactly one form.
constexpr Vector kRejected[] = {
#define NODE_ID_STRING_CANONICAL NODE_ID_STRING_NONE
#define NODE_ID_STRING_WRITTEN NODE_ID_STRING_NONE
#define NODE_ID_STRING_READ NODE_ID_STRING_NONE
#define NODE_ID_STRING_REJECTED NODE_ID_STRING_REJECTED_ROW
#include "scada/node_id_string_vectors.inc"
#undef NODE_ID_STRING_CANONICAL
#undef NODE_ID_STRING_WRITTEN
#undef NODE_ID_STRING_READ
#undef NODE_ID_STRING_REJECTED
};

ByteString FromHex(std::string_view hex) {
  ByteString bytes;
  for (size_t i = 0; i + 1 < hex.size(); i += 2)
    bytes.push_back(static_cast<char>(
        std::stoi(std::string{hex.substr(i, 2)}, nullptr, 16)));
  return bytes;
}

// The NodeId a vector stands for.
NodeId NodeIdOf(const Vector& vector) {
  switch (vector.kind) {
    case 'i':
      return NodeId{
          static_cast<NumericId>(std::stoul(std::string{vector.value})),
          vector.namespace_index};
    case 's':
      return NodeId{String{vector.value}, vector.namespace_index};
    case 'g':
      return NodeId{*Guid::FromString(vector.value), vector.namespace_index};
    default:
      return NodeId{FromHex(vector.value), vector.namespace_index};
  }
}

TEST(NodeIdStringTest, TheSharedVectorsAreThere) {
  EXPECT_GE(std::size(kCanonical), 10u);
  EXPECT_GE(std::size(kWritten), 1u);
  EXPECT_GE(std::size(kRead), 5u);
  EXPECT_GE(std::size(kRejected), 10u);
}

TEST(NodeIdStringTest, FormatWritesTheCanonicalForm) {
  for (const Vector& vector : kCanonical) {
    SCOPED_TRACE(vector.text);
    EXPECT_EQ(FormatNodeIdString(NodeIdOf(vector), vector.namespace_uri),
              vector.text);
  }
  for (const Vector& vector : kWritten) {
    SCOPED_TRACE(vector.text);
    EXPECT_EQ(FormatNodeIdString(NodeIdOf(vector), vector.namespace_uri),
              vector.text);
  }
}

void ExpectReads(const Vector& vector) {
  SCOPED_TRACE(vector.text);
  const std::optional<ParsedNodeIdString> parsed =
      ParseNodeIdString(vector.text);
  ASSERT_TRUE(parsed.has_value());
  EXPECT_EQ(parsed->node_id, NodeIdOf(vector));
  if (vector.namespace_uri.empty()) {
    EXPECT_FALSE(parsed->namespace_uri.has_value());
  } else {
    EXPECT_EQ(parsed->namespace_uri, std::string{vector.namespace_uri});
  }
}

TEST(NodeIdStringTest, ParseReadsTheCanonicalForm) {
  for (const Vector& vector : kCanonical)
    ExpectReads(vector);
}

TEST(NodeIdStringTest, ParseReadsTheOtherForms) {
  for (const Vector& vector : kRead)
    ExpectReads(vector);
}

TEST(NodeIdStringTest, RejectsWhatIsNotExactlyOneForm) {
  for (const Vector& vector : kRejected) {
    SCOPED_TRACE(vector.text);
    EXPECT_FALSE(ParseNodeIdString(vector.text).has_value());
  }
}

TEST(NodeIdStringTest, FormatIgnoresTheNodeIdsOwnIndex) {
  // The URI names the namespace; a session's index for it is never written.
  EXPECT_EQ(FormatNodeIdString(NodeId{7u, 38}, "urn:x"), "nsu=urn:x;i=7");
  EXPECT_EQ(FormatNodeIdString(NodeId{2258u, 5}, ""), "i=2258");
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
