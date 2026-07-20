#pragma once

// Namespace-index remapping between two servers' index spaces.
//
// A NodeId's NamespaceIndex is only an index into the NamespaceArray of the
// server that issued it, so it is server-local: the same index means different
// namespaces on different servers, and the same namespace may have different
// indexes. Anything carrying NodeIds across that divide must translate them.
//
// This lives in the core scada layer, not in the OPC UA bridge, because it is
// pure NodeId/QualifiedName/Variant vocabulary with no dependency on the opcua::
// type universe. It has three consumers:
//   - an Aggregating Server re-exposing several Aggregated Servers behind one
//     address space (Build, against a growing ProxyNamespaceTable);
//   - a tier translating at its own serving boundary so it can publish only the
//     namespaces it serves (ADR 0003);
//   - an OPC UA *client* talking to a server whose array need not match its own
//     (BuildByUri).
// The direction names keep the original proxy framing: "proxy" is the local /
// fixed index space and "downstream" the remote one.
//
// OPC UA Part 3 §8.2.3 NamespaceArray,
// https://reference.opcfoundation.org/Core/Part3/v105/docs/8.2.3 ; Part 1
// §5.3.7 server-to-server / aggregation,
// https://reference.opcfoundation.org/Core/Part1/v105/docs/5.3 .

#include "scada/proxy_namespace_table.h"

#include "scada/basic_types.h"
#include "scada/expanded_node_id.h"
#include "scada/node_id.h"
#include "scada/qualified_name.h"
#include "scada/variant.h"
#include "scada/view_service.h"

#include <span>
#include <string>
#include <unordered_map>
#include <vector>

namespace scada::aggregation {

// ProxyNamespaceTable and kOpcUaNamespaceUri live in
// scada/proxy_namespace_table.h (shared with the core module that serves
// Server_NamespaceArray).

// Bidirectional NamespaceIndex translation between one Aggregated (downstream)
// Server and the Aggregating (proxy) Server. Translates the NodeIds /
// ExpandedNodeIds / QualifiedNames that flow across the proxy boundary:
//   - ToProxy   : downstream -> proxy, for responses returned to the client.
//   - ToDownstream : proxy -> downstream, for requests sent to the Aggregated
//     Server.
// Identifiers in namespaces this remapper does not own are returned unchanged
// (OwnsProxyNamespace lets a router decide which downstream a request targets).
class NamespaceRemapper {
 public:
  NamespaceRemapper() = default;

  // Builds the maps from a downstream server's NamespaceArray, where
  // `downstream_uris[i]` is the URI at the downstream's NamespaceIndex i (read
  // from Server_NamespaceArray, i=2255). Each downstream URI is merged into
  // `proxy_table`; the matching proxy index becomes this downstream's mapping.
  static NamespaceRemapper Build(std::span<const std::string> downstream_uris,
                                 ProxyNamespaceTable& proxy_table);

  // Builds the maps from explicit (downstream index, proxy index) pairs;
  // indexes absent from `pairs` stay identity-mapped. Used when the proxy
  // side's indexes are FIXED (e.g. a configuration consumer aligning a hosted
  // configuration's namespaces with its local canonical indexes) rather than
  // allocated from a table.
  static NamespaceRemapper BuildFromPairs(
      std::span<const std::pair<scada::NamespaceIndex /*downstream*/,
                                scada::NamespaceIndex /*proxy*/>> pairs);

  // Builds the remapper an OPC UA *client* applies to a remote server whose
  // NamespaceArray need not match the client's own index space — which is any
  // tier that publishes only the namespaces it serves (ADR 0003).
  //
  // Both arrays are supplied by the caller and matched by URI equality:
  // `remote_uris` is the server's Server_NamespaceArray (i=2255), `local_uris`
  // the client's own. Nothing here is compiled in — a client learns a server's
  // namespaces from what that server publishes, which is the whole point, since
  // assuming they match is the defect this remapper exists to fix.
  //
  // The client's indexes take the "proxy" role and the server's the
  // "downstream" one, so
  //   - ToDownstream : local -> remote, for a request sent to the server, and
  //   - ToProxy      : remote -> local, for the response,
  // which is the polarity the RemappingXxxService wrappers already apply. A
  // remote URI absent from `local_uris` gets no mapping and is left
  // untranslated, the same treatment any unknown namespace receives.
  //
  // OPC UA Part 3 §8.2.3 makes the URI the stable identity,
  // https://reference.opcfoundation.org/Core/Part3/v105/docs/8.2.3 .
  static NamespaceRemapper BuildByUri(std::span<const std::string> local_uris,
                                      std::span<const std::string> remote_uris);

  // True if `proxy_index` denotes a namespace this downstream server owns, i.e.
  // a proxy->downstream translation is defined for it.
  bool OwnsProxyNamespace(scada::NamespaceIndex proxy_index) const;

  // downstream -> proxy
  scada::NodeId ToProxy(const scada::NodeId& node_id) const;
  scada::ExpandedNodeId ToProxy(const scada::ExpandedNodeId& node_id) const;
  scada::QualifiedName ToProxy(const scada::QualifiedName& name) const;
  scada::ReferenceDescription ToProxy(
      const scada::ReferenceDescription& reference) const;
  // Remaps any NodeId / ExpandedNodeId / QualifiedName carried as a value
  // (scalar or array) inside `value`; other value types are returned unchanged.
  // Translates identifier-typed attribute/notification values and method
  // arguments across the proxy boundary.
  scada::Variant ToProxy(const scada::Variant& value) const;

  // proxy -> downstream
  scada::NodeId ToDownstream(const scada::NodeId& node_id) const;
  scada::ExpandedNodeId ToDownstream(
      const scada::ExpandedNodeId& node_id) const;
  scada::QualifiedName ToDownstream(const scada::QualifiedName& name) const;
  scada::Variant ToDownstream(const scada::Variant& value) const;

 private:
  scada::NamespaceIndex MapToProxy(
      scada::NamespaceIndex downstream_index) const;
  scada::NamespaceIndex MapToDownstream(
      scada::NamespaceIndex proxy_index) const;

  // to_proxy_[downstream_index] = proxy_index.
  std::vector<scada::NamespaceIndex> to_proxy_;
  // proxy_index -> downstream_index, for the namespaces this downstream owns.
  std::unordered_map<scada::NamespaceIndex, scada::NamespaceIndex>
      to_downstream_;
};

}  // namespace scada::aggregation
