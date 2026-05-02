#pragma once

#include <memory>

#include "base/boost_log.h"
#include "scada/node_management_service.h"

class MessageSender;

class NodeManagementProxy : public scada::NodeManagementService {
 public:
  void OnChannelOpened(MessageSender& sender);
  void OnChannelClosed();

  // scada::NodeManagementService
  Awaitable<scada::StatusOr<std::vector<scada::AddNodesResult>>> AddNodes(
      std::vector<scada::AddNodesItem> inputs) override;
  Awaitable<scada::StatusOr<std::vector<scada::StatusCode>>> DeleteNodes(
      std::vector<scada::DeleteNodesItem> inputs) override;
  Awaitable<scada::StatusOr<std::vector<scada::StatusCode>>> AddReferences(
      std::vector<scada::AddReferencesItem> inputs) override;
  Awaitable<scada::StatusOr<std::vector<scada::StatusCode>>> DeleteReferences(
      std::vector<scada::DeleteReferencesItem> inputs) override;

 private:
  BoostLogger logger_{LOG_NAME("NodeManagementProxy")};

  MessageSender* sender_ = nullptr;
};
