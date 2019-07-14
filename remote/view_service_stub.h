#pragma once

#include "base/logger.h"
#include "base/memory/weak_ptr.h"
#include "base/timer.h"

#include <memory>

namespace protocol {
class Reference;
class Request;
}  // namespace protocol

namespace scada {
class ViewService;
struct BrowseDescription;
}

class MessageSender;

struct ViewServiceStubContext {
  const std::shared_ptr<Logger> logger_;
  boost::asio::io_context& io_context_;
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

  Timer timer_;

  base::WeakPtrFactory<ViewServiceStub> weak_factory_;
};
