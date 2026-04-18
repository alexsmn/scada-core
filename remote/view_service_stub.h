#pragma once

#include "base/boost_log.h"
#include "scada/service_context.h"
#include "base/awaitable.h"
#include "scada/view_service.h"

#include <memory>

namespace google::protobuf {
template <typename T>
class RepeatedPtrField;
}

namespace protocol {
class Request;
}  // namespace protocol

class Executor;
class MessageSender;

struct ViewServiceStubContext {
  const std::shared_ptr<Executor> executor_;
  const std::weak_ptr<MessageSender> sender_;
  const scada::ServiceContext service_context_;
  scada::ViewService& service_;
};

class ViewServiceStub final : private ViewServiceStubContext,
                              public std::enable_shared_from_this<ViewServiceStub> {
 public:
  explicit ViewServiceStub(ViewServiceStubContext&& context);
  ~ViewServiceStub();

  void OnRequestReceived(const protocol::Request& request);

 private:
  void OnBrowse(const protocol::Request& request);
  [[nodiscard]] Awaitable<void> OnBrowseAsync(
      unsigned request_id,
      scada::ServiceContext context,
      std::vector<scada::BrowseDescription> inputs);
  void OnBrowsePaths(const protocol::Request& request);
 [[nodiscard]] Awaitable<void> OnBrowsePathsAsync(
      unsigned request_id,
      std::vector<scada::BrowsePath> inputs);

  BoostLogger logger_{LOG_NAME("ViewServiceStub")};
};
