#pragma once

#include "base/timer.h"

#include <memory>

namespace protocol {
class Reference;
class Request;
}  // namespace protocol

namespace scada {
class ViewService;
struct BrowseDescription;
}  // namespace scada

class MessageSender;

struct ViewServiceStubContext {
  boost::asio::io_context& io_context_;
  MessageSender& sender_;
  scada::ViewService& service_;
};

class ViewServiceStub final
    : private ViewServiceStubContext,
      public std::enable_shared_from_this<ViewServiceStub> {
 public:
  explicit ViewServiceStub(ViewServiceStubContext&& context);
  ~ViewServiceStub();

  void OnRequestReceived(const protocol::Request& request);

 private:
  void OnBrowse(unsigned request_id,
                const std::vector<scada::BrowseDescription>& nodes);
  void OnBrowsePaths(const protocol::Request& request);

  Timer timer_;
};
