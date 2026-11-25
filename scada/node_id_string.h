#pragma once

// The self-contained text form of a NodeId: the one that names its namespace
// by URI ("nsu=") so that it means the same node in any session and on any
// server, and may therefore be stored — in a display binding, a formula or a
// saved workspace. OPC UA Part 6 §5.1.12 QualifiedName, NodeId and
// ExpandedNodeId String Encoding, Table 5,
// https://reference.opcfoundation.org/Core/Part6/v105/docs/5.1.12
//
// This layer knows no namespace table: it returns the URI the text names, and
// the caller maps it to its own index.

#include "scada/node_id.h"
#include "scada/string.h"

#include <optional>
#include <string_view>

namespace scada {

// A NodeId string, parsed, with the namespace named the way the text named it.
struct ParsedNodeIdString {
  // The namespace URI, decoded, when the text named one ("nsu="). Empty for
  // the bare form and the "ns=" form, and for "nsu=" naming the OPC UA
  // namespace, which is read as the bare form.
  std::optional<String> namespace_uri;

  // The node. Its namespace index is the "ns=" value when the text gave one,
  // and 0 otherwise — for "nsu=" the caller sets the index its own table gives
  // `namespace_uri` (NodeId::set_namespace_index).
  NodeId node_id;
};

// Parses the Part 6 §5.1.12 NodeId string forms: `<identifier>`,
// `ns=<index>;<identifier>` and `nsu=<encoded uri>;<identifier>`, where the
// identifier is `i=`, `s=`, `g=` or `b=`. Prefixes match in any case, as ABNF
// literals do (RFC 5234 §2.3). Unlike NodeId::FromString, which reads any
// unknown text as a string identifier, this returns nullopt for anything that
// is not exactly one of those forms: a numeric id that is empty or out of
// range, an index past 65535, a malformed Guid, Base64 or percent escape, an
// empty URI, or no identifier prefix.
std::optional<ParsedNodeIdString> ParseNodeIdString(std::string_view text);

// Writes `node_id` in the "nsu=" form, naming its namespace by
// `namespace_uri`; the NodeId's own index is not written, since the URI is
// what names the namespace. An empty URI or the OPC UA namespace's URI writes
// the bare identifier, as Part 6 §5.1.12 requires for namespace 0.
// ParseNodeIdString reads the result back to the same identifier and URI.
String FormatNodeIdString(const NodeId& node_id,
                          std::string_view namespace_uri);

// Percent-encodes a namespace URI for the "nsu=" form (RFC 3986 §2.1). Every
// byte outside RFC 3986's unreserved and reserved sets is encoded, and so are
// ';', which Part 6 §5.1.12 adds to the reserved set, and '%' itself, so that
// any string — including one that is not a well-formed URI, or that already
// holds an escape — decodes back to exactly itself. Hex digits are upper case.
String EncodeNamespaceUri(std::string_view uri);

// The inverse of EncodeNamespaceUri: decodes every `%XX` escape (either case).
// Returns nullopt for a '%' not followed by two hex digits.
std::optional<String> DecodeNamespaceUri(std::string_view encoded);

}  // namespace scada
