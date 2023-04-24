#pragma once

#include "base/struct_writer.h"
#include "core/node_attributes.h"
#include "core/node_class.h"
#include "core/status.h"
#include "core/status_callback.h"

#include <functional>
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
  StatusCode status_code = StatusCode::Bad;
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

using AddNodesCallback =
    std::function<void(Status&& status, std::vector<AddNodesResult>&& results)>;
using DeleteNodesCallback =
    std::function<void(Status&& status, std::vector<StatusCode>&& results)>;

using AddReferencesCallback = MultiStatusCallback;
using DeleteReferencesCallback = MultiStatusCallback;

class NodeManagementService {
 public:
  virtual ~NodeManagementService() = default;

  virtual void AddNodes(const std::vector<AddNodesItem>& inputs,
                        const AddNodesCallback& callback) = 0;

  // Delete record from table. If |return_dependencies| is true and deletion
  // fails, it gets list of related records, which must be deleted before.
  virtual void DeleteNodes(const std::vector<DeleteNodesItem>& inputs,
                           const DeleteNodesCallback& callback) = 0;

  virtual void AddReferences(const std::vector<AddReferencesItem>& inputs,
                             const AddReferencesCallback& callback) = 0;

  virtual void DeleteReferences(const std::vector<DeleteReferencesItem>& inputs,
                                const DeleteReferencesCallback& callback) = 0;
};

// using Callback = std::function<void(AddNodesResult&& result)>
template <class Callback>
inline void AddNode(NodeManagementService& service,
                    const AddNodesItem& input,
                    Callback&& callback) {
  service.AddNodes(
      {input}, [callback](Status&& status,
                          std::vector<AddNodesResult>&& results) mutable {
        auto result =
            status ? std::move(results[0]) : AddNodesResult{status.code()};
        callback(std::move(result));
      });
}

// using Callback = std::function<void(Status&& result)>
template <class Callback>
inline void DeleteNode(NodeManagementService& service,
                       const DeleteNodesItem& input,
                       Callback&& callback) {
  service.DeleteNodes(
      {input},
      [callback](Status&& status, std::vector<StatusCode>&& results) mutable {
        auto result = status ? scada::Status{results[0]} : std::move(status);
        callback(std::move(result));
      });
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
