#pragma once

// Service-boundary wrappers that apply NamespaceRemapper translation around a
// service belonging to another server's index space: every NodeId /
// QualifiedName / ExpandedNodeId in a request is translated proxy -> downstream
// before the call, and every one in the response is translated downstream ->
// proxy. This is the per-request half of OPC UA aggregation (Part 1 §5.3.7);
// the namespace mapping itself lives in NamespaceRemapper (Part 3 §8.2.3).
//
// Scope: structural identifiers (request targets, Browse reference
// descriptions, browse-path targets, monitored-item targets, method/object ids)
// are remapped, as are identifier-typed *values* carried inside
// read/notification DataValues, write values, and method arguments (NodeId /
// ExpandedNodeId / QualifiedName, scalar or array). Event-notification payloads
// (std::any) are the one remaining untranslated surface.

#include "scada/namespace_remapper.h"

#include "scada/attribute_service.h"
#include "scada/history_service.h"
#include "scada/history_update_service.h"
#include "scada/method_service.h"
#include "scada/monitored_item_service.h"
#include "scada/node_management_service.h"
#include "scada/view_service.h"

namespace scada::aggregation {

// Wraps a downstream ViewService, remapping Browse / TranslateBrowsePaths.
class RemappingViewService : public scada::ViewService {
 public:
  RemappingViewService(scada::ViewService& inner,
                       const NamespaceRemapper& remapper)
      : inner_{inner}, remapper_{remapper} {}

  Awaitable<scada::StatusOr<std::vector<scada::BrowseResult>>> Browse(
      scada::ServiceContext context,
      std::vector<scada::BrowseDescription> inputs) override;

  Awaitable<scada::StatusOr<std::vector<scada::BrowsePathResult>>>
  TranslateBrowsePaths(std::vector<scada::BrowsePath> inputs) override;

 private:
  scada::ViewService& inner_;
  const NamespaceRemapper& remapper_;
};

// Wraps a downstream AttributeService, remapping Read / Write targets.
class RemappingAttributeService : public scada::AttributeService {
 public:
  RemappingAttributeService(scada::AttributeService& inner,
                            const NamespaceRemapper& remapper)
      : inner_{inner}, remapper_{remapper} {}

  Awaitable<scada::StatusOr<std::vector<scada::DataValue>>> Read(
      scada::ServiceContext context,
      std::vector<scada::ReadValueId> inputs) override;

  Awaitable<scada::StatusOr<std::vector<scada::StatusCode>>> Write(
      scada::ServiceContext context,
      std::vector<scada::WriteValue> inputs) override;

 private:
  scada::AttributeService& inner_;
  const NamespaceRemapper& remapper_;
};

// Wraps a downstream HistoryService, remapping the requested source NodeId
// (proxy -> downstream) and any identifier-typed values carried back in the
// historical DataValues (downstream -> proxy). Event history remaps the source
// NodeId only; event payloads (std::any) are the same untranslated surface as
// the monitored-item notifications.
class RemappingHistoryService : public scada::HistoryService {
 public:
  RemappingHistoryService(scada::HistoryService& inner,
                          const NamespaceRemapper& remapper)
      : inner_{inner}, remapper_{remapper} {}

  Awaitable<scada::StatusOr<scada::HistoryReadRawResult>> HistoryReadRaw(
      scada::HistoryReadRawDetails details) override;

  Awaitable<scada::StatusOr<scada::HistoryReadEventsResult>> HistoryReadEvents(
      scada::NodeId node_id,
      scada::Time from,
      scada::Time to,
      scada::EventFilter filter) override;

 private:
  scada::HistoryService& inner_;
  const NamespaceRemapper& remapper_;
};

// Wraps a downstream HistoryUpdateService, remapping the updated node's NodeId
// and any identifier-typed values carried in the written DataValues (proxy ->
// downstream). The results are plain StatusCodes, so nothing needs translating
// back.
//
// The aggregating proxy does not forward HistoryUpdate, so this wrapper exists
// for a tier's own serving boundary (ADR 0003): without it
// `history_update_service_` would cross untranslated and write history against
// wrong-namespace NodeIds. OPC UA Part 4 §5.10.5 HistoryUpdate,
// https://reference.opcfoundation.org/Core/Part4/v105/docs/5.10.5 .
class RemappingHistoryUpdateService : public scada::HistoryUpdateService {
 public:
  RemappingHistoryUpdateService(scada::HistoryUpdateService& inner,
                                const NamespaceRemapper& remapper)
      : inner_{inner}, remapper_{remapper} {}

  Awaitable<scada::StatusOr<std::vector<scada::StatusCode>>> HistoryUpdateData(
      scada::ServiceContext context,
      scada::UpdateDataDetails details) override;

  Awaitable<scada::StatusOr<std::vector<scada::StatusCode>>> HistoryUpdateEvent(
      scada::ServiceContext context,
      scada::UpdateEventDetails details) override;

 private:
  scada::HistoryUpdateService& inner_;
  const NamespaceRemapper& remapper_;
};

// Wraps a downstream MethodService, remapping the called object id, method id,
// and any identifier-typed input argument values.
class RemappingMethodService : public scada::MethodService {
 public:
  RemappingMethodService(scada::MethodService& inner,
                         const NamespaceRemapper& remapper)
      : inner_{inner}, remapper_{remapper} {}

  Awaitable<scada::Status> Call(scada::NodeId node_id,
                                scada::NodeId method_id,
                                std::vector<scada::Variant> arguments,
                                scada::ServiceContext context) override;

 private:
  scada::MethodService& inner_;
  const NamespaceRemapper& remapper_;
};

// Wraps a downstream MonitoredItemService; the subscriptions it hands back
// remap each monitored item's target NodeId (proxy -> downstream) as items are
// added, and translate identifier-typed data-change values (downstream ->
// proxy) in the notifications they deliver. Event-notification payloads
// (std::any) are not translated.
class RemappingMonitoredItemService : public scada::MonitoredItemService {
 public:
  RemappingMonitoredItemService(scada::MonitoredItemService& inner,
                                const NamespaceRemapper& remapper)
      : inner_{inner}, remapper_{remapper} {}

  scada::StatusOr<std::unique_ptr<scada::MonitoredItemSubscription>>
  CreateSubscription(scada::ServiceContext context,
                     scada::MonitoredItemSubscriptionOptions options) override;

 private:
  scada::MonitoredItemService& inner_;
  const NamespaceRemapper& remapper_;
};

// Wraps a downstream NodeManagementService, remapping every identifier in the
// AddNodes / DeleteNodes / AddReferences / DeleteReferences requests
// (proxy -> downstream) and the AddNodes result ids (downstream -> proxy).
// This is the write half of file management through the proxy: a client's
// AddNodes of a FileType node is forwarded to the downstream that owns the
// file store. OPC UA Part 4 §5.7 NodeManagement,
// https://reference.opcfoundation.org/Core/Part4/v105/docs/5.7 .
class RemappingNodeManagementService : public scada::NodeManagementService {
 public:
  RemappingNodeManagementService(scada::NodeManagementService& inner,
                                 const NamespaceRemapper& remapper)
      : inner_{inner}, remapper_{remapper} {}

  Awaitable<scada::StatusOr<std::vector<scada::AddNodesResult>>> AddNodes(
      scada::ServiceContext context,
      std::vector<scada::AddNodesItem> inputs) override;

  Awaitable<scada::StatusOr<std::vector<scada::StatusCode>>> DeleteNodes(
      scada::ServiceContext context,
      std::vector<scada::DeleteNodesItem> inputs) override;

  Awaitable<scada::StatusOr<std::vector<scada::StatusCode>>> AddReferences(
      scada::ServiceContext context,
      std::vector<scada::AddReferencesItem> inputs) override;

  Awaitable<scada::StatusOr<std::vector<scada::StatusCode>>> DeleteReferences(
      scada::ServiceContext context,
      std::vector<scada::DeleteReferencesItem> inputs) override;

 private:
  scada::NodeManagementService& inner_;
  const NamespaceRemapper& remapper_;
};

// Surfaces an Aggregated Server's top-level nodes under the proxy's Objects
// folder. A Browse of the (ns 0) Objects folder is forwarded to the downstream
// and its non-standard children — those in the downstream's own namespaces, not
// ns 0 — are returned. The router merges and de-duplicates these into the
// proxy's own Objects-folder browse, so each downstream's roots appear as mount
// points; the downstream's standard nodes (its Server, Types, ...) are dropped
// because the proxy serves ns 0 itself. OPC UA Part 1 §5.3.7 aggregation.
class MountViewService : public scada::ViewService {
 public:
  explicit MountViewService(scada::ViewService& inner) : inner_{inner} {}

  Awaitable<scada::StatusOr<std::vector<scada::BrowseResult>>> Browse(
      scada::ServiceContext context,
      std::vector<scada::BrowseDescription> inputs) override;

  Awaitable<scada::StatusOr<std::vector<scada::BrowsePathResult>>>
  TranslateBrowsePaths(std::vector<scada::BrowsePath> inputs) override;

 private:
  // The downstream's already namespace-remapped ViewService.
  scada::ViewService& inner_;
};

}  // namespace scada::aggregation
