#pragma once

#include "base/boost_log.h"
#include "scada/node_attributes.h"
#include "scada/node_id.h"
#include "scada/node_management_service.h"

#include <memory>
#include <vector>

namespace protocol {
class Reference;
class Request;
}  // namespace protocol

namespace scada {
struct ServiceContext;
}

class Executor;
class MessageSender;

class NodeManagementStub {
 public:
  NodeManagementStub(
      std::shared_ptr<Executor> executor,
      std::weak_ptr<MessageSender> sender,
      scada::NodeManagementService& service,
      std::shared_ptr<const scada::ServiceContext> service_context);

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

  const std::shared_ptr<Executor> executor_;
  const std::weak_ptr<MessageSender> sender_;

  scada::NodeManagementService& service_;

  const std::shared_ptr<BoostLogger> logger_ =
      std::make_shared<BoostLogger>(LOG_NAME("NodeManagementStub"));

  // Identifier of logged user for access control.
  const std::shared_ptr<const scada::ServiceContext> service_context_;
};
