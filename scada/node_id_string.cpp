#include "scada/node_id_string.h"

#include "base/base64.h"
#include "scada/guid.h"
#include "scada/proxy_namespace_table.h"

#include <boost/algorithm/string/predicate.hpp>

#include <charconv>
#include <limits>
#include <string>

namespace scada {

namespace {

constexpr char kHexDigits[] = "0123456789ABCDEF";

// RFC 3986 §2.2 reserved and §2.3 unreserved characters, minus ';' (Part 6
// §5.1.12 adds it to the reserved set) — the bytes a "nsu=" URI keeps as they
// are. https://www.rfc-editor.org/rfc/rfc3986#section-2
bool IsKeptInUri(char c) {
  if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
      (c >= '0' && c <= '9'))
    return true;
  switch (c) {
    case '-':
    case '.':
    case '_':
    case '~':  // unreserved
    case ':':
    case '/':
    case '?':
    case '#':
    case '[':
    case ']':
    case '@':  // gen-delims
    case '!':
    case '$':
    case '&':
    case '\'':
    case '(':
    case ')':
    case '*':
    case '+':
    case ',':
    case '=':  // sub-delims
      return true;
    default:
      return false;
  }
}

int HexValue(char c) {
  if (c >= '0' && c <= '9')
    return c - '0';
  if (c >= 'A' && c <= 'F')
    return c - 'A' + 10;
  if (c >= 'a' && c <= 'f')
    return c - 'a' + 10;
  return -1;
}

// An unsigned decimal of at least one digit that fits in `T`, and nothing
// else: from_chars alone would accept a prefix.
template <class T>
std::optional<T> ParseDecimal(std::string_view text) {
  if (text.empty())
    return std::nullopt;
  T value = 0;
  const auto [end, error] =
      std::from_chars(text.data(), text.data() + text.size(), value);
  if (error != std::errc{} || end != text.data() + text.size())
    return std::nullopt;
  return value;
}

// The Base64 text decoded, only when it is the canonical encoding of what it
// decodes to: Base64Decode stops quietly at the first byte outside the
// alphabet, which would read "b=AB$CD" as a shorter id.
std::optional<ByteString> ParseBase64(std::string_view text) {
  std::string decoded;
  base::Base64Decode(text, &decoded);
  std::string reencoded;
  base::Base64Encode(decoded, &reencoded);
  if (reencoded != text)
    return std::nullopt;
  return ByteString{decoded.begin(), decoded.end()};
}

// `<identifier>` of Part 6 §5.1.12 Table 5, in `namespace_index`.
std::optional<NodeId> ParseIdentifier(std::string_view text,
                                      NamespaceIndex namespace_index) {
  if (text.size() < 2 || text[1] != '=')
    return std::nullopt;
  const std::string_view value = text.substr(2);
  switch (text[0]) {
    case 'i':
    case 'I':
      if (const auto id = ParseDecimal<NumericId>(value))
        return NodeId{*id, namespace_index};
      return std::nullopt;
    case 's':
    case 'S':
      return NodeId{String{value}, namespace_index};
    case 'g':
    case 'G':
      if (const std::optional<Guid> guid = Guid::FromString(value))
        return NodeId{*guid, namespace_index};
      return std::nullopt;
    case 'b':
    case 'B':
      if (std::optional<ByteString> bytes = ParseBase64(value))
        return NodeId{std::move(*bytes), namespace_index};
      return std::nullopt;
    default:
      return std::nullopt;
  }
}

}  // namespace

std::optional<ParsedNodeIdString> ParseNodeIdString(std::string_view text) {
  ParsedNodeIdString result;
  NamespaceIndex namespace_index = 0;

  // "nsu=" before "ns=": the one is a prefix of the other.
  if (boost::istarts_with(text, "nsu=")) {
    const size_t semicolon = text.find(';');
    if (semicolon == std::string_view::npos)
      return std::nullopt;
    std::optional<String> uri =
        DecodeNamespaceUri(text.substr(4, semicolon - 4));
    if (!uri || uri->empty())
      return std::nullopt;
    if (*uri != aggregation::kOpcUaNamespaceUri)
      result.namespace_uri = std::move(*uri);
    text = text.substr(semicolon + 1);
  } else if (boost::istarts_with(text, "ns=")) {
    const size_t semicolon = text.find(';');
    if (semicolon == std::string_view::npos)
      return std::nullopt;
    const auto index =
        ParseDecimal<NamespaceIndex>(text.substr(3, semicolon - 3));
    if (!index)
      return std::nullopt;
    namespace_index = *index;
    text = text.substr(semicolon + 1);
  }

  std::optional<NodeId> node_id = ParseIdentifier(text, namespace_index);
  if (!node_id)
    return std::nullopt;
  result.node_id = std::move(*node_id);
  return result;
}

String FormatNodeIdString(const NodeId& node_id,
                          std::string_view namespace_uri) {
  // NodeId::ToString writes the bare identifier for namespace 0.
  NodeId identifier = node_id;
  identifier.set_namespace_index(0);
  if (namespace_uri.empty() || namespace_uri == aggregation::kOpcUaNamespaceUri)
    return identifier.ToString();
  return "nsu=" + EncodeNamespaceUri(namespace_uri) + ";" +
         identifier.ToString();
}

String EncodeNamespaceUri(std::string_view uri) {
  String encoded;
  encoded.reserve(uri.size());
  for (const char c : uri) {
    if (IsKeptInUri(c)) {
      encoded += c;
    } else {
      const auto byte = static_cast<unsigned char>(c);
      encoded += '%';
      encoded += kHexDigits[byte >> 4];
      encoded += kHexDigits[byte & 0x0F];
    }
  }
  return encoded;
}

std::optional<String> DecodeNamespaceUri(std::string_view encoded) {
  String decoded;
  decoded.reserve(encoded.size());
  for (size_t i = 0; i < encoded.size(); ++i) {
    if (encoded[i] != '%') {
      decoded += encoded[i];
      continue;
    }
    if (i + 2 >= encoded.size())
      return std::nullopt;
    const int high = HexValue(encoded[i + 1]);
    const int low = HexValue(encoded[i + 2]);
    if (high < 0 || low < 0)
      return std::nullopt;
    decoded += static_cast<char>(high * 16 + low);
    i += 2;
  }
  return decoded;
}

}  // namespace scada
