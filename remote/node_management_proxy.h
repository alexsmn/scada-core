#pragma once

#include <memory>

#include "core/node_management_service.h"

class Logger;
class MessageSender;

class NodeManagementProxy : public scada::NodeManagementService {
 public:
  explicit NodeManagementProxy(std::shared_ptr<Logger> logger);

  void OnChannelOpened(MessageSender& sender);
  void OnChannelClosed();

  // scada::NodeManagementService
  virtual void AddNodes(const std::vector<scada::AddNodesItem>& inputs,
                        const scada::AddNodesCallback& callback) override;
  virtual void DeleteNodes(const std::vector<scada::DeleteNodesItem>& inputs,
                           const scada::DeleteNodesCallback& callback) override;
  virtual void ChangeUserPassword(
      const scada::NodeId& user_node_id,
      const scada::LocalizedText& current_password,
      const scada::LocalizedText& new_password,
      const scada::StatusCallback& callback) override;
  virtual void AddReferences(
      const std::vector<scada::AddReferencesItem>& inputs,
      const scada::AddReferencesCallback& callback) override;
  virtual void DeleteReferences(
      const std::vector<scada::DeleteReferencesItem>& inputs,
      const scada::DeleteReferencesCallback& callback) override;

 private:
  Logger& logger() { return *logger_; }

  std::shared_ptr<Logger> logger_;

  MessageSender* sender_ = nullptr;
};
