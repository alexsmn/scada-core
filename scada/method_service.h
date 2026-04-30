#pragma once

#include "base/any_executor.h"
#include "base/awaitable.h"
#include "base/executor_conversions.h"
#include "scada/callback_awaitable.h"
#include "scada/node_id.h"
#include "scada/status.h"
#include "scada/status_callback.h"
#include "scada/status_exception.h"
#include "scada/variant.h"

#include <functional>
#include <memory>
#include <utility>
#include <vector>

namespace scada {

class MethodService {
 public:
  virtual ~MethodService() = default;

  virtual void Call(const NodeId& node_id,
                    const NodeId& method_id,
                    const std::vector<Variant>& arguments,
                    const scada::NodeId& user_id,
                    const StatusCallback& callback) = 0;
};

class CoroutineMethodService {
 public:
  virtual ~CoroutineMethodService() = default;

  virtual Awaitable<Status> Call(NodeId node_id,
                                 NodeId method_id,
                                 std::vector<Variant> arguments,
                                 NodeId user_id) = 0;
};

class CallbackToCoroutineMethodServiceAdapter final
    : public CoroutineMethodService {
 public:
  CallbackToCoroutineMethodServiceAdapter(AnyExecutor executor,
                                          MethodService& service)
      : executor_{std::move(executor)}, service_{service} {}
  CallbackToCoroutineMethodServiceAdapter(std::shared_ptr<Executor> executor,
                                          MethodService& service)
      : CallbackToCoroutineMethodServiceAdapter(
            MakeAnyExecutor(std::move(executor)), service) {}

  Awaitable<Status> Call(NodeId node_id,
                         NodeId method_id,
                         std::vector<Variant> arguments,
                         NodeId user_id) override {
    co_return co_await AwaitStatusCallback(
        executor_, [this, node_id = std::move(node_id),
                    method_id = std::move(method_id),
                    arguments = std::move(arguments),
                    user_id = std::move(user_id)](auto callback) mutable {
          service_.Call(node_id, method_id, arguments, user_id,
                        std::move(callback));
        });
  }

 private:
  const AnyExecutor executor_;
  MethodService& service_;
};

class CoroutineToCallbackMethodServiceAdapter final : public MethodService {
 public:
  CoroutineToCallbackMethodServiceAdapter(AnyExecutor executor,
                                          CoroutineMethodService& service)
      : executor_{std::move(executor)}, service_{service} {}
  CoroutineToCallbackMethodServiceAdapter(std::shared_ptr<Executor> executor,
                                          CoroutineMethodService& service)
      : CoroutineToCallbackMethodServiceAdapter(
            MakeAnyExecutor(std::move(executor)), service) {}

  void Call(const NodeId& node_id,
            const NodeId& method_id,
            const std::vector<Variant>& arguments,
            const scada::NodeId& user_id,
            const StatusCallback& callback) override {
    CoSpawn(executor_,
            [this, node_id, method_id, arguments, user_id,
             callback]() mutable -> Awaitable<void> {
              try {
                auto status = co_await service_.Call(node_id, method_id,
                                                     arguments, user_id);
                callback(std::move(status));
              } catch (...) {
                auto status = GetExceptionStatus(std::current_exception());
                callback(std::move(status));
              }
            });
  }

 private:
  const AnyExecutor executor_;
  CoroutineMethodService& service_;
};

}  // namespace scada
