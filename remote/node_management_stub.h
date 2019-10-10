#pragma once

#include "base/boost_log.h"
#include "base/memory/weak_ptr.h"
#include "core/node_attributes.h"
#include "core/node_id.h"

#include <memory>
#include <vector>

namespace protocol {
class Reference;
class Request;
}  // namespace protocol

namespace scada {
class NodeManagementService;
enum class NodeClass;
}  // namespace scada

class MessageSender;

class NodeManagementStub {
 public:
  NodeManagementStub(MessageSender& sender,
                     scada::NodeManagementService& service,
                     scada::NodeId user_id);

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
  void OnAddReference(unsigned request_id, const protocol::Reference& request);
  void OnDeleteReference(unsigned request_id,
                         const protocol::Reference& request);

  MessageSender& sender_;
  scada::NodeManagementService& service_;
  // Identifier of logged user for access control.
  const scada::NodeId user_id_;

  BoostLogger logger_{LOG_NAME("NodeManagementStub")};

  base::WeakPtrFactory<NodeManagementStub> weak_factory_{this};
};
