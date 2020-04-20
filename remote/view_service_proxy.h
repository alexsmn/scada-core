#pragma once

#include <memory>

#include "core/view_service.h"

namespace protocol {
class BrowseResult;
}  // namespace protocol

class Logger;
class MessageSender;

class ViewServiceProxy : public scada::ViewService {
 public:
  explicit ViewServiceProxy(std::shared_ptr<Logger> logger);

  void OnChannelOpened(MessageSender& sender);
  void OnChannelClosed();

  // scada::ViewService
  virtual void Browse(const std::vector<scada::BrowseDescription>& nodes,
                      const scada::BrowseCallback& callback) override;
  virtual void TranslateBrowsePath(
      const scada::NodeId& starting_node_id,
      const scada::RelativePath& relative_path,
      const scada::TranslateBrowsePathCallback& callback) override;

 private:
  const std::shared_ptr<Logger> logger_;

  MessageSender* sender_ = nullptr;
};
