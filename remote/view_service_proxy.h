#pragma once

#include <memory>

#include "core/view_service.h"

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
  virtual void TranslateBrowsePaths(
      const std::vector<scada::BrowsePath>& browse_paths,
      const scada::TranslateBrowsePathsCallback& callback) override;

 private:
  const std::shared_ptr<Logger> logger_;

  MessageSender* sender_ = nullptr;
};
