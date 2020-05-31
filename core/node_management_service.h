#pragma once

#include "core/configuration_types.h"
#include "core/node_attributes.h"
#include "core/node_class.h"
#include "core/status.h"

#include <functional>
#include <vector>

namespace scada {

enum class NodeClass;

struct AddReferencesItem {
  NodeId source_node_id;
  NodeId reference_type_id;
  bool forward = true;
  String target_server_uri;
  ExpandedNodeId target_node_id;
  NodeClass target_node_class = scada::NodeClass::Object;
};

struct DeleteReferencesItem {
  NodeId source_node_id;
  NodeId reference_type_id;
  bool forward = true;
  ExpandedNodeId target_node_id;
  bool delete_bidirectional = true;
};

using StatusCallback = std::function<void(Status&&)>;
using MultiStatusCallback =
    std::function<void(Status&&, std::vector<StatusCode>&&)>;

using CreateNodeCallback =
    std::function<void(Status&& status, const NodeId& node_id)>;
using DeleteNodeCallback =
    std::function<void(Status&& status,
                       std::vector<scada::NodeId>&& dependencies)>;

using AddReferencesCallback = MultiStatusCallback;
using DeleteReferencesCallback = MultiStatusCallback;

class NodeManagementService {
 public:
  virtual ~NodeManagementService() {}

  virtual void CreateNode(const NodeId& requested_id,
                          const NodeId& parent_id,
                          NodeClass node_class,
                          const NodeId& type_id,
                          NodeAttributes attributes,
                          const CreateNodeCallback& callback) = 0;

  // Delete record from table. If |return_dependencies| is true and deletion
  // fails, it gets list of related records, which must be deleted before.
  virtual void DeleteNode(const NodeId& node_id,
                          bool return_dependencies,
                          const DeleteNodeCallback& callback) = 0;

  virtual void ChangeUserPassword(const NodeId& user_node_id,
                                  const LocalizedText& current_password,
                                  const LocalizedText& new_password,
                                  const StatusCallback& callback) = 0;

  virtual void AddReferences(const std::vector<AddReferencesItem>& inputs,
                             const AddReferencesCallback& callback) = 0;

  virtual void DeleteReferences(const std::vector<DeleteReferencesItem>& inputs,
                                const DeleteReferencesCallback& callback) = 0;
};

}  // namespace scada
