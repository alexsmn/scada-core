#pragma once

#include "base/memory/weak_ptr.h"
#include "core/node_attributes.h"
#include "core/node_id.h"
#include "core/node_management_service.h"

#include <memory>
#include <vector>

namespace protocol {
class Reference;
class Request;
}  // namespace protocol

class Logger;
class MessageSender;

class NodeManagementStub {
 public:
  NodeManagementStub(MessageSender& sender,
                     scada::NodeManagementService& service,
                     const scada::NodeId& user_id,
                     std::shared_ptr<Logger> logger);

  void OnRequestReceived(const protocol::Request& request);

 private:
  void OnCreateNode(unsigned request_id,
                    const scada::NodeId& requested_id,
                    const scada::NodeId& parent_id,
                    scada::NodeClass node_class,
                    const scada::NodeId& type_id,
                    scada::NodeAttributes attributes);
  void OnDeleteNode(unsigned request_id,
                    const scada::NodeId& id,
                    bool return_dependencies);
  void OnChangeUserPassword(unsigned request_id,
                            const scada::NodeId& user_node_id,
                            const scada::LocalizedText& current_password,
                            const scada::LocalizedText& new_password);
  void OnAddReferences(unsigned request_id,
                       const std::vector<scada::AddReferencesItem>& inputs);
  void OnDeleteReferences(
      unsigned request_id,
      const std::vector<scada::DeleteReferencesItem>& inputs);

  MessageSender& sender_;

  scada::NodeManagementService& service_;

  // Identifier of logged user for access control.
  scada::NodeId user_id_;

  const std::shared_ptr<Logger> logger_;

  base::WeakPtrFactory<NodeManagementStub> weak_factory_{this};
};
