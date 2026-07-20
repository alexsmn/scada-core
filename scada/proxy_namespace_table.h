#pragma once

// The server's live NamespaceArray as a growing, de-duplicated URI table. For a
// plain server this is just its static namespaces; for an OPC UA Aggregating
// Server it is seeded with those static namespaces and then extended with each
// Aggregated Server's namespaces (which is why it lives in server/base, shared
// by the core module that serves Server_NamespaceArray and the aggregation
// module that extends it). It is pure NodeId-vocabulary, so it sits in the core
// scada layer rather than the OPC UA bridge. A namespace whose URI already
// appears maps to the
// existing index, so shared namespaces merge; a new URI is appended and
// assigned a fresh index. OPC UA Part 3 §8.2.3 NamespaceArray,
// https://reference.opcfoundation.org/Core/Part3/v105/docs/8.2.3 .

#include "base/lifetime.h"
#include "scada/basic_types.h"

#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace scada::aggregation {

// The OPC UA standard namespace URI, always NamespaceIndex 0.
inline constexpr std::string_view kOpcUaNamespaceUri =
    "http://opcfoundation.org/UA/";

class ProxyNamespaceTable {
 public:
  // Seeds index 0 with the OPC UA standard namespace only.
  ProxyNamespaceTable();
  // Seeds the table with `uris` verbatim (uris[i] is the URI at index i), e.g.
  // scada::model::GetCanonicalNamespaceUris(); subsequent GetOrAdd appends after them.
  explicit ProxyNamespaceTable(std::vector<std::string> uris);

  // Returns the index for `uri`, appending it (and a new index) if absent.
  scada::NamespaceIndex GetOrAdd(std::string_view uri);

  // Returns the index for `uri`, or nullopt if absent.
  std::optional<scada::NamespaceIndex> Find(std::string_view uri) const;

  // The NamespaceArray (uris()[i] is the URI at NamespaceIndex i).
  const std::vector<std::string>& uris() const SCADA_LIFETIME_BOUND {
    return uris_;
  }

 private:
  std::vector<std::string> uris_;
};

}  // namespace scada::aggregation
