#pragma once

#include "scada/service_context.h"
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

class ViewServiceStub final : private ViewServiceStubContext {
 public:
  explicit ViewServiceStub(ViewServiceStubContext&& context);
  ~ViewServiceStub();

  void OnRequestReceived(const protocol::Request& request);

 private:
  void OnBrowse(const protocol::Request& request);
  void OnBrowsePaths(const protocol::Request& request);
};
