#pragma once

#include "base/logger.h"
#include "base/memory/weak_ptr.h"
#include "core/view_service.h"

namespace protocol {
class Reference;
class Request;
}  // namespace protocol

class MessageSender;

struct ViewServiceStubContext {
  const std::shared_ptr<Logger> logger_;
  MessageSender& sender_;
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

  base::WeakPtrFactory<ViewServiceStub> weak_factory_{this};
};
