#pragma once

#include "base/boost_log.h"
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

class MessageSender;

class NodeManagementStub {
 public:
  NodeManagementStub(MessageSender& sender,
                     scada::NodeManagementService& service,
                     const scada::NodeId& user_id);

  void OnRequestReceived(const protocol::Request& request);

 private:
  void OnAddNodes(unsigned request_id,
                  const std::vector<scada::AddNodesItem>& inputs);
  void OnDeleteNodes(unsigned request_id,
                     const std::vector<scada::DeleteNodesItem>& inputs);
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

  BoostLogger logger_{LOG_NAME("NodeManagementStub")};

  // Identifier of logged user for access control.
  scada::NodeId user_id_;

  base::WeakPtrFactory<NodeManagementStub> weak_factory_{this};
};
