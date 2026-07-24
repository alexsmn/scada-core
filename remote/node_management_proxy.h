#pragma once

#include <memory>

#include "base/boost_log.h"
#include "scada/co_result.h"
#include "scada/node_management_service.h"

class MessageSender;

class NodeManagementProxy : public scada::NodeManagementService {
 public:
  void OnChannelOpened(MessageSender& sender);
  void OnChannelClosed();

  // scada::NodeManagementService
  scada::CoStatusOr<std::vector<scada::AddNodesResult>> AddNodes(
      scada::ServiceContext context,
      std::vector<scada::AddNodesItem> inputs) override;
  scada::CoStatusOr<std::vector<scada::StatusCode>> DeleteNodes(
      scada::ServiceContext context,
      std::vector<scada::DeleteNodesItem> inputs) override;
  scada::CoStatusOr<std::vector<scada::StatusCode>> AddReferences(
      scada::ServiceContext context,
      std::vector<scada::AddReferencesItem> inputs) override;
  scada::CoStatusOr<std::vector<scada::StatusCode>> DeleteReferences(
      scada::ServiceContext context,
      std::vector<scada::DeleteReferencesItem> inputs) override;

 private:
  BoostLogger logger_{LOG_NAME("NodeManagementProxy")};

  MessageSender* sender_ = nullptr;
};
