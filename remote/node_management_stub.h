#pragma once

#include "base/boost_log.h"
#include "base/awaitable.h"
#include "scada/coroutine_services.h"
#include "scada/node_attributes.h"
#include "scada/node_id.h"
#include "scada/node_management_service.h"
#include "scada/service_context.h"

#include <memory>
#include <vector>

namespace protocol {
class Reference;
class Request;
}  // namespace protocol

namespace scada {
class CoroutineNodeManagementService;
class ServiceContext;
}

class Executor;
class MessageSender;

class NodeManagementStub : public std::enable_shared_from_this<NodeManagementStub> {
 public:
  NodeManagementStub(std::shared_ptr<Executor> executor,
                     std::weak_ptr<MessageSender> sender,
                     scada::CoroutineNodeManagementService& coroutine_service,
                     const scada::ServiceContext& service_context);

  void OnRequestReceived(const protocol::Request& request);

 private:
  void OnAddNodes(unsigned request_id,
                  const std::vector<scada::AddNodesItem>& inputs);
  [[nodiscard]] Awaitable<void> OnAddNodesAsync(
      unsigned request_id,
      std::vector<scada::AddNodesItem> inputs);
  void OnDeleteNodes(unsigned request_id,
                     const std::vector<scada::DeleteNodesItem>& inputs);
  [[nodiscard]] Awaitable<void> OnDeleteNodesAsync(
      unsigned request_id,
      std::vector<scada::DeleteNodesItem> inputs);
  void OnChangeUserPassword(unsigned request_id,
                            const scada::NodeId& user_node_id,
                            const scada::LocalizedText& current_password,
                            const scada::LocalizedText& new_password);
  void OnAddReferences(unsigned request_id,
                       const std::vector<scada::AddReferencesItem>& inputs);
  [[nodiscard]] Awaitable<void> OnAddReferencesAsync(
      unsigned request_id,
      std::vector<scada::AddReferencesItem> inputs);
  void OnDeleteReferences(
      unsigned request_id,
      const std::vector<scada::DeleteReferencesItem>& inputs);
  [[nodiscard]] Awaitable<void> OnDeleteReferencesAsync(
      unsigned request_id,
      std::vector<scada::DeleteReferencesItem> inputs);

  const std::shared_ptr<Executor> executor_;
  const std::weak_ptr<MessageSender> sender_;

  scada::CoroutineNodeManagementService& coroutine_service_;

  const std::shared_ptr<BoostLogger> logger_ =
      std::make_shared<BoostLogger>(LOG_NAME("NodeManagementStub"));

  // Identifier of logged user for access control.
  const scada::ServiceContext service_context_;
};
