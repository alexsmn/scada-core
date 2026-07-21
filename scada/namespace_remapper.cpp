#include "scada/namespace_remapper.h"

#include "base/boost_log.h"
#include "scada/event.h"

#include <algorithm>
#include <utility>
#include <vector>

namespace scada::aggregation {

namespace {

BoostLogger& Logger() {
  static BoostLogger logger{LOG_NAME("Aggregation")};
  return logger;
}

}  // namespace

// static
NamespaceRemapper NamespaceRemapper::Build(
    std::span<const std::string> downstream_uris,
    ProxyNamespaceTable& proxy_table) {
  NamespaceRemapper remapper;
  remapper.to_proxy_.resize(downstream_uris.size());
  std::size_t empty_uri_count = 0;
  for (std::size_t downstream_index = 0;
       downstream_index < downstream_uris.size(); ++downstream_index) {
    // A downstream slot without a URI identifies no namespace; leave it
    // identity-mapped instead of merging every empty slot onto the table's
    // first empty entry. OPC UA Part 3 §8.2.3 requires NamespaceArray entries
    // to be namespace URIs, so an empty slot is a downstream defect (e.g. a
    // pre-fix build whose static table still had empty entries).
    // https://reference.opcfoundation.org/Core/Part3/v105/docs/8.2.3
    if (downstream_uris[downstream_index].empty()) {
      ++empty_uri_count;
      remapper.to_proxy_[downstream_index] =
          static_cast<scada::NamespaceIndex>(downstream_index);
      continue;
    }
    const scada::NamespaceIndex proxy_index =
        proxy_table.GetOrAdd(downstream_uris[downstream_index]);
    remapper.to_proxy_[downstream_index] = proxy_index;
    remapper.to_downstream_.emplace(
        proxy_index, static_cast<scada::NamespaceIndex>(downstream_index));
  }
  if (empty_uri_count > 0) {
    LOG_WARNING(Logger())
        << "Downstream NamespaceArray has empty namespace URIs; leaving them "
           "identity-mapped (OPC UA Part 3 §8.2.3 requires a URI per entry — "
           "likely a stale downstream build)"
        << LOG_TAG("EmptyUriCount", empty_uri_count)
        << LOG_TAG("NamespaceCount", downstream_uris.size());
  }
  return remapper;
}

// static
NamespaceRemapper NamespaceRemapper::BuildFromPairs(
    std::span<const std::pair<scada::NamespaceIndex, scada::NamespaceIndex>>
        pairs) {
  NamespaceRemapper remapper;
  scada::NamespaceIndex max_downstream = 0;
  for (const auto& [downstream_index, proxy_index] : pairs) {
    max_downstream = std::max(max_downstream, downstream_index);
  }
  remapper.to_proxy_.resize(static_cast<std::size_t>(max_downstream) + 1);
  for (std::size_t i = 0; i < remapper.to_proxy_.size(); ++i) {
    remapper.to_proxy_[i] = static_cast<scada::NamespaceIndex>(i);
  }
  for (const auto& [downstream_index, proxy_index] : pairs) {
    remapper.to_proxy_[downstream_index] = proxy_index;
    remapper.to_downstream_.emplace(proxy_index, downstream_index);
  }
  return remapper;
}

// static
NamespaceRemapper NamespaceRemapper::BuildByUri(
    std::span<const std::string> local_uris,
    std::span<const std::string> remote_uris) {
  // (downstream, proxy) = (remote, local): the server's published index space
  // is the "downstream" side and the client's own is the "proxy" side.
  std::vector<std::pair<scada::NamespaceIndex, scada::NamespaceIndex>> pairs;
  for (std::size_t remote = 0; remote < remote_uris.size(); ++remote) {
    if (remote_uris[remote].empty()) {
      continue;
    }
    const auto local =
        std::find(local_uris.begin(), local_uris.end(), remote_uris[remote]);
    if (local == local_uris.end()) {
      // A namespace the client does not know: leave it untranslated rather than
      // guess an index for it.
      continue;
    }
    pairs.emplace_back(
        static_cast<scada::NamespaceIndex>(remote),
        static_cast<scada::NamespaceIndex>(local - local_uris.begin()));
  }
  return BuildFromPairs(pairs);
}

bool NamespaceRemapper::OwnsProxyNamespace(
    scada::NamespaceIndex proxy_index) const {
  return to_downstream_.contains(proxy_index);
}

scada::NamespaceIndex NamespaceRemapper::MapToProxy(
    scada::NamespaceIndex downstream_index) const {
  if (downstream_index < to_proxy_.size()) {
    return to_proxy_[downstream_index];
  }
  return downstream_index;  // Unknown downstream namespace: leave untranslated.
}

scada::NamespaceIndex NamespaceRemapper::MapToDownstream(
    scada::NamespaceIndex proxy_index) const {
  const auto it = to_downstream_.find(proxy_index);
  if (it != to_downstream_.end()) {
    return it->second;
  }
  return proxy_index;  // Not this downstream's namespace: leave untranslated.
}

namespace {

// Returns `node_id` with its NamespaceIndex replaced by `mapped`.
scada::NodeId WithNamespace(scada::NodeId node_id,
                            scada::NamespaceIndex mapped) {
  node_id.set_namespace_index(mapped);
  return node_id;
}

}  // namespace

scada::NodeId NamespaceRemapper::ToProxy(const scada::NodeId& node_id) const {
  return WithNamespace(node_id, MapToProxy(node_id.namespace_index()));
}

scada::NodeId NamespaceRemapper::ToDownstream(
    const scada::NodeId& node_id) const {
  return WithNamespace(node_id, MapToDownstream(node_id.namespace_index()));
}

scada::QualifiedName NamespaceRemapper::ToProxy(
    const scada::QualifiedName& name) const {
  return {name.name(), MapToProxy(name.namespace_index())};
}

scada::QualifiedName NamespaceRemapper::ToDownstream(
    const scada::QualifiedName& name) const {
  return {name.name(), MapToDownstream(name.namespace_index())};
}

scada::ExpandedNodeId NamespaceRemapper::ToProxy(
    const scada::ExpandedNodeId& node_id) const {
  // When a namespace URI is present it identifies the namespace directly and is
  // server-independent, so the index is not authoritative and is left as-is.
  if (!node_id.namespace_uri().empty()) {
    return node_id;
  }
  return {ToProxy(node_id.node_id()), node_id.namespace_uri(),
          node_id.server_index()};
}

scada::ExpandedNodeId NamespaceRemapper::ToDownstream(
    const scada::ExpandedNodeId& node_id) const {
  if (!node_id.namespace_uri().empty()) {
    return node_id;
  }
  return {ToDownstream(node_id.node_id()), node_id.namespace_uri(),
          node_id.server_index()};
}

scada::ReferenceDescription NamespaceRemapper::ToProxy(
    const scada::ReferenceDescription& reference) const {
  scada::ReferenceDescription result = reference;
  result.reference_type_id = ToProxy(reference.reference_type_id);
  result.node_id = ToProxy(reference.node_id);
  result.browse_name = ToProxy(reference.browse_name);
  result.type_definition = ToProxy(reference.type_definition);
  return result;
}

namespace {

// Returns `value` with every identifier it carries (scalar or array of NodeId /
// ExpandedNodeId / QualifiedName) passed through `remap`; other value types are
// returned unchanged. `remap` is a generic callable that dispatches by argument
// type to the matching ToProxy/ToDownstream overload.
template <class Remap>
scada::Variant RemapVariant(const scada::Variant& value, Remap remap) {
  if (const auto* id = value.get_if<scada::NodeId>()) {
    return scada::Variant{remap(*id)};
  }
  if (const auto* id = value.get_if<scada::ExpandedNodeId>()) {
    return scada::Variant{remap(*id)};
  }
  if (const auto* name = value.get_if<scada::QualifiedName>()) {
    return scada::Variant{remap(*name)};
  }
  if (const auto* ids = value.get_if<std::vector<scada::NodeId>>()) {
    std::vector<scada::NodeId> out;
    out.reserve(ids->size());
    for (const auto& id : *ids) {
      out.push_back(remap(id));
    }
    return scada::Variant{std::move(out)};
  }
  if (const auto* ids = value.get_if<std::vector<scada::ExpandedNodeId>>()) {
    std::vector<scada::ExpandedNodeId> out;
    out.reserve(ids->size());
    for (const auto& id : *ids) {
      out.push_back(remap(id));
    }
    return scada::Variant{std::move(out)};
  }
  if (const auto* names = value.get_if<std::vector<scada::QualifiedName>>()) {
    std::vector<scada::QualifiedName> out;
    out.reserve(names->size());
    for (const auto& name : *names) {
      out.push_back(remap(name));
    }
    return scada::Variant{std::move(out)};
  }
  return value;
}

}  // namespace

scada::Variant NamespaceRemapper::ToProxy(const scada::Variant& value) const {
  return RemapVariant(value, [this](const auto& id) { return ToProxy(id); });
}

scada::Variant NamespaceRemapper::ToDownstream(
    const scada::Variant& value) const {
  return RemapVariant(value,
                      [this](const auto& id) { return ToDownstream(id); });
}

scada::Event NamespaceRemapper::ToProxy(const scada::Event& event) const {
  scada::Event result = event;
  result.event_type_id = ToProxy(event.event_type_id);
  result.source_node_id = ToProxy(event.source_node_id);
  result.user_id = ToProxy(event.user_id);
  result.acknowledged_user_id = ToProxy(event.acknowledged_user_id);
  result.value = ToProxy(event.value);
  return result;
}

scada::ModelChangeEvent NamespaceRemapper::ToProxy(
    const scada::ModelChangeEvent& event) const {
  // event_type_id is a class constant (ns0), never remapped.
  scada::ModelChangeEvent result = event;
  result.node_id = ToProxy(event.node_id);
  result.type_definition_id = ToProxy(event.type_definition_id);
  return result;
}

scada::SemanticChangeEvent NamespaceRemapper::ToProxy(
    const scada::SemanticChangeEvent& event) const {
  scada::SemanticChangeEvent result = event;
  result.node_id = ToProxy(event.node_id);
  return result;
}

}  // namespace scada::aggregation
