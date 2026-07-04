#pragma once

#include "base/any_executor.h"
#include "base/awaitable.h"
#include "base/struct_writer.h"
#include "scada/node_attributes.h"
#include "scada/node_class.h"
#include "scada/service_context.h"
#include "scada/status.h"
#include "scada/status_or.h"

#include <utility>
#include <vector>

namespace scada {

struct AddNodesItem {
  NodeId requested_id;
  NodeId parent_id;
  NodeClass node_class = NodeClass::Object;
  NodeId type_definition_id;
  NodeAttributes attributes;
};

struct AddNodesResult {
  StatusCode status_code = StatusCode::Good;
  NodeId added_node_id;
};

struct DeleteNodesItem {
  NodeId node_id;
  bool delete_target_references = false;
};

struct AddReferencesItem {
  NodeId source_node_id;
  NodeId reference_type_id;
  bool forward = true;
  String target_server_uri;
  ExpandedNodeId target_node_id;
  NodeClass target_node_class = NodeClass::Object;
};

struct DeleteReferencesItem {
  NodeId source_node_id;
  NodeId reference_type_id;
  bool forward = true;
  ExpandedNodeId target_node_id;
  bool delete_bidirectional = true;
};

class NodeManagementService {
 public:
  virtual ~NodeManagementService() = default;

  // `context` carries the caller identity and rights for authorization
  // (OPC UA Part 4 §5.7 NodeManagement).
  virtual Awaitable<StatusOr<std::vector<AddNodesResult>>> AddNodes(
      ServiceContext context,
      std::vector<AddNodesItem> inputs) = 0;

  // Delete record from table. If |return_dependencies| is true and deletion
  // fails, it gets list of related records, which must be deleted before.
  virtual Awaitable<StatusOr<std::vector<StatusCode>>> DeleteNodes(
      ServiceContext context,
      std::vector<DeleteNodesItem> inputs) = 0;

  virtual Awaitable<StatusOr<std::vector<StatusCode>>> AddReferences(
      ServiceContext context,
      std::vector<AddReferencesItem> inputs) = 0;

  virtual Awaitable<StatusOr<std::vector<StatusCode>>> DeleteReferences(
      ServiceContext context,
      std::vector<DeleteReferencesItem> inputs) = 0;
};

// The single-item convenience helpers are client-side (the server session
// authorizes), so they pass an empty context.
inline Awaitable<AddNodesResult> AddNode(NodeManagementService& service,
                                         AddNodesItem input) {
  std::vector<AddNodesItem> inputs;
  inputs.emplace_back(std::move(input));
  auto results = co_await service.AddNodes(ServiceContext{}, std::move(inputs));
  if (!results.ok()) {
    co_return AddNodesResult{.status_code = results.status().code()};
  }
  assert(results->size() == 1);
  co_return std::move(results->front());
}

inline Awaitable<Status> DeleteNode(NodeManagementService& service,
                                    DeleteNodesItem input) {
  std::vector<DeleteNodesItem> inputs;
  inputs.emplace_back(std::move(input));
  auto results =
      co_await service.DeleteNodes(ServiceContext{}, std::move(inputs));
  if (!results.ok()) {
    co_return results.status();
  }
  assert(results->size() == 1);
  co_return Status{results->front()};
}

inline bool operator==(const AddNodesItem& a, const AddNodesItem& b) {
  return a.requested_id == b.requested_id && a.parent_id == b.parent_id &&
         a.node_class == b.node_class &&
         a.type_definition_id == b.type_definition_id &&
         a.attributes == b.attributes;
}

inline std::ostream& operator<<(std::ostream& stream,
                                const AddNodesItem& item) {
  StructWriter{stream}
      .AddField("requested_id", item.requested_id)
      .AddField("parent_id", item.parent_id)
      .AddField("node_class", item.node_class)
      .AddField("type_definition_id", item.type_definition_id)
      .AddField("attributes", item.attributes);
  return stream;
}

}  // namespace scada
