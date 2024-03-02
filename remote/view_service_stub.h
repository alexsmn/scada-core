#pragma once

#include "scada/view_service.h"

#include <memory>

namespace protocol {
class Reference;
class Request;
}  // namespace protocol

class Executor;

class MessageSender;

struct ViewServiceStubContext {
  const std::shared_ptr<Executor> executor_;
  const std::weak_ptr<MessageSender> sender_;
  const std::shared_ptr<const scada::ServiceContext> service_context_;
  scada::ViewService& service_;
};

class ViewServiceStub final : private ViewServiceStubContext {
 public:
  explicit ViewServiceStub(ViewServiceStubContext&& context);
  ~ViewServiceStub();

  void OnRequestReceived(const protocol::Request& request);

 private:
  void OnBrowse(unsigned request_id,
                const std::vector<scada::BrowseDescription>& nodes);

  void OnBrowsePaths(const protocol::Request& request);
};
