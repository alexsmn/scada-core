#pragma once

#include <memory>

#include "base/boost_log.h"
#include "core/node_management_service.h"

class MessageSender;

class NodeManagementProxy : public scada::NodeManagementService {
 public:
  void OnChannelOpened(MessageSender& sender);
  void OnChannelClosed();

  // scada::NodeManagementService
  virtual void AddNodes(const std::vector<scada::AddNodesItem>& inputs,
                        const scada::AddNodesCallback& callback) override;
  virtual void DeleteNodes(const std::vector<scada::DeleteNodesItem>& inputs,
                           const scada::DeleteNodesCallback& callback) override;
  virtual void AddReferences(
      const std::vector<scada::AddReferencesItem>& inputs,
      const scada::AddReferencesCallback& callback) override;
  virtual void DeleteReferences(
      const std::vector<scada::DeleteReferencesItem>& inputs,
      const scada::DeleteReferencesCallback& callback) override;

 private:
  BoostLogger logger_{LOG_NAME("NodeManagementProxy")};

  MessageSender* sender_ = nullptr;
};
