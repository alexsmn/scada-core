#include "scada/proxy_namespace_table.h"

#include <algorithm>
#include <string_view>
#include <utility>

namespace scada::aggregation {

ProxyNamespaceTable::ProxyNamespaceTable()
    : uris_{std::string{kOpcUaNamespaceUri}} {}

ProxyNamespaceTable::ProxyNamespaceTable(std::vector<std::string> uris)
    : uris_{std::move(uris)} {}

scada::NamespaceIndex ProxyNamespaceTable::GetOrAdd(std::string_view uri) {
  if (const auto existing = Find(uri); existing.has_value()) {
    return *existing;
  }
  uris_.emplace_back(uri);
  return static_cast<scada::NamespaceIndex>(uris_.size() - 1);
}

std::optional<scada::NamespaceIndex> ProxyNamespaceTable::Find(
    std::string_view uri) const {
  const auto it = std::find(uris_.begin(), uris_.end(), uri);
  if (it == uris_.end()) {
    return std::nullopt;
  }
  return static_cast<scada::NamespaceIndex>(it - uris_.begin());
}

}  // namespace scada::aggregation
