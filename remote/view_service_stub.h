#pragma once

#include "core/view_service.h"

#include <memory>

class Executor;

namespace protocol {
class Reference;
class Request;
}  // namespace protocol

class MessageSender;

struct ViewServiceStubContext {
  const std::shared_ptr<Executor> executor_;
  const std::weak_ptr<MessageSender> sender_;
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
};
