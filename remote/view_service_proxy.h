#pragma once

#include "scada/view_service.h"

#include <memory>

class MessageSender;

class ViewServiceProxy : public scada::ViewService {
 public:
  void OnChannelOpened(MessageSender& sender);
  void OnChannelClosed();

  // scada::ViewService
  [[nodiscard]] Awaitable<scada::StatusOr<std::vector<scada::BrowseResult>>>
  Browse(scada::ServiceContext context,
         std::vector<scada::BrowseDescription> nodes) override;
  [[nodiscard]] Awaitable<scada::StatusOr<std::vector<scada::BrowsePathResult>>>
  TranslateBrowsePaths(std::vector<scada::BrowsePath> browse_paths) override;

 private:
  MessageSender* sender_ = nullptr;
};
