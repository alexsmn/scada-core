#pragma once

#include "base/any_executor.h"
#include "base/awaitable.h"
#include "base/awaitable_promise.h"
#include "base/callback_awaitable.h"
#include "base/executor_conversions.h"
#include "scada/attribute_service.h"
#include "scada/history_service.h"
#include "scada/method_service.h"
#include "scada/node_management_service.h"
#include "scada/service_context.h"
#include "scada/session_service.h"
#include "scada/status_exception.h"
#include "scada/view_service.h"

#include <memory>
#include <tuple>
#include <utility>
#include <vector>

namespace scada {

class CoroutineAttributeService {
 public:
  virtual ~CoroutineAttributeService() = default;

  virtual Awaitable<std::tuple<Status, std::vector<DataValue>>> Read(
      ServiceContext context,
      std::shared_ptr<const std::vector<ReadValueId>> inputs) = 0;

  virtual Awaitable<std::tuple<Status, std::vector<StatusCode>>> Write(
      ServiceContext context,
      std::shared_ptr<const std::vector<WriteValue>> inputs) = 0;
};

class CoroutineMethodService {
 public:
  virtual ~CoroutineMethodService() = default;

  virtual Awaitable<Status> Call(NodeId node_id,
                                 NodeId method_id,
                                 std::vector<Variant> arguments,
                                 NodeId user_id) = 0;
};

class CoroutineHistoryService {
 public:
  virtual ~CoroutineHistoryService() = default;

  virtual Awaitable<HistoryReadRawResult> HistoryReadRaw(
      HistoryReadRawDetails details) = 0;

  virtual Awaitable<HistoryReadEventsResult> HistoryReadEvents(
      NodeId node_id,
      base::Time from,
      base::Time to,
      EventFilter filter) = 0;
};

class CoroutineViewService {
 public:
  virtual ~CoroutineViewService() = default;

  virtual Awaitable<std::tuple<Status, std::vector<BrowseResult>>> Browse(
      ServiceContext context,
      std::vector<BrowseDescription> inputs) = 0;

  virtual Awaitable<std::tuple<Status, std::vector<BrowsePathResult>>>
  TranslateBrowsePaths(std::vector<BrowsePath> inputs) = 0;
};

class CoroutineNodeManagementService {
 public:
  virtual ~CoroutineNodeManagementService() = default;

  virtual Awaitable<std::tuple<Status, std::vector<AddNodesResult>>> AddNodes(
      std::vector<AddNodesItem> inputs) = 0;

  virtual Awaitable<std::tuple<Status, std::vector<StatusCode>>> DeleteNodes(
      std::vector<DeleteNodesItem> inputs) = 0;

  virtual Awaitable<std::tuple<Status, std::vector<StatusCode>>>
  AddReferences(std::vector<AddReferencesItem> inputs) = 0;

  virtual Awaitable<std::tuple<Status, std::vector<StatusCode>>>
  DeleteReferences(std::vector<DeleteReferencesItem> inputs) = 0;
};

class CoroutineSessionService {
 public:
  virtual ~CoroutineSessionService() = default;

  virtual Awaitable<void> Connect(SessionConnectParams params) = 0;
  virtual Awaitable<void> Reconnect() = 0;
  virtual Awaitable<void> Disconnect() = 0;

  virtual bool IsConnected(base::TimeDelta* ping_delay = nullptr) const = 0;
  virtual NodeId GetUserId() const = 0;
  virtual bool HasPrivilege(Privilege privilege) const = 0;
  virtual std::string GetHostName() const = 0;
  virtual bool IsScada() const = 0;

  using SessionStateChangedCallback =
      SessionService::SessionStateChangedCallback;

  virtual boost::signals2::scoped_connection SubscribeSessionStateChanged(
      const SessionStateChangedCallback& callback) = 0;

  virtual SessionDebugger* GetSessionDebugger() = 0;
};

class CallbackToCoroutineAttributeServiceAdapter final
    : public CoroutineAttributeService {
 public:
  CallbackToCoroutineAttributeServiceAdapter(
      AnyExecutor executor,
      AttributeService& service)
      : executor_{std::move(executor)}, service_{service} {}
  CallbackToCoroutineAttributeServiceAdapter(
      std::shared_ptr<Executor> executor,
      AttributeService& service)
      : CallbackToCoroutineAttributeServiceAdapter(
            MakeAnyExecutor(std::move(executor)), service) {}

  Awaitable<std::tuple<Status, std::vector<DataValue>>> Read(
      ServiceContext context,
      std::shared_ptr<const std::vector<ReadValueId>> inputs) override {
    co_return co_await CallbackToAwaitable<Status, std::vector<DataValue>>(
        executor_,
        [this, context = std::move(context), inputs = std::move(inputs)](
            auto callback) mutable {
          service_.Read(context, std::move(inputs), std::move(callback));
        });
  }

  Awaitable<std::tuple<Status, std::vector<StatusCode>>> Write(
      ServiceContext context,
      std::shared_ptr<const std::vector<WriteValue>> inputs) override {
    co_return co_await CallbackToAwaitable<Status, std::vector<StatusCode>>(
        executor_,
        [this, context = std::move(context), inputs = std::move(inputs)](
            auto callback) mutable {
          service_.Write(context, std::move(inputs), std::move(callback));
        });
  }

 private:
  const AnyExecutor executor_;
  AttributeService& service_;
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
    auto [status] = co_await CallbackToAwaitable<Status>(
        executor_, [this, node_id = std::move(node_id),
                    method_id = std::move(method_id),
                    arguments = std::move(arguments),
                    user_id = std::move(user_id)](auto callback) mutable {
          service_.Call(node_id, method_id, arguments, user_id,
                        std::move(callback));
        });
    co_return std::move(status);
  }

 private:
  const AnyExecutor executor_;
  MethodService& service_;
};

class CallbackToCoroutineHistoryServiceAdapter final
    : public CoroutineHistoryService {
 public:
  CallbackToCoroutineHistoryServiceAdapter(AnyExecutor executor,
                                           HistoryService& service)
      : executor_{std::move(executor)}, service_{service} {}
  CallbackToCoroutineHistoryServiceAdapter(std::shared_ptr<Executor> executor,
                                           HistoryService& service)
      : CallbackToCoroutineHistoryServiceAdapter(
            MakeAnyExecutor(std::move(executor)), service) {}

  Awaitable<HistoryReadRawResult> HistoryReadRaw(
      HistoryReadRawDetails details) override {
    auto [result] = co_await CallbackToAwaitable<HistoryReadRawResult>(
        executor_,
        [this, details = std::move(details)](auto callback) mutable {
          service_.HistoryReadRaw(details, std::move(callback));
        });
    co_return std::move(result);
  }

  Awaitable<HistoryReadEventsResult> HistoryReadEvents(
      NodeId node_id,
      base::Time from,
      base::Time to,
      EventFilter filter) override {
    auto [result] = co_await CallbackToAwaitable<HistoryReadEventsResult>(
        executor_,
        [this, node_id = std::move(node_id), from, to,
         filter = std::move(filter)](auto callback) mutable {
          service_.HistoryReadEvents(node_id, from, to, filter,
                                     std::move(callback));
        });
    co_return std::move(result);
  }

 private:
  const AnyExecutor executor_;
  HistoryService& service_;
};

class CallbackToCoroutineViewServiceAdapter final : public CoroutineViewService {
 public:
  CallbackToCoroutineViewServiceAdapter(AnyExecutor executor,
                                        ViewService& service)
      : executor_{std::move(executor)}, service_{service} {}
  CallbackToCoroutineViewServiceAdapter(std::shared_ptr<Executor> executor,
                                        ViewService& service)
      : CallbackToCoroutineViewServiceAdapter(
            MakeAnyExecutor(std::move(executor)), service) {}

  Awaitable<std::tuple<Status, std::vector<BrowseResult>>> Browse(
      ServiceContext context,
      std::vector<BrowseDescription> inputs) override {
    co_return co_await CallbackToAwaitable<Status, std::vector<BrowseResult>>(
        executor_,
        [this, context = std::move(context), inputs = std::move(inputs)](
            auto callback) mutable {
          service_.Browse(context, inputs, std::move(callback));
        });
  }

  Awaitable<std::tuple<Status, std::vector<BrowsePathResult>>>
  TranslateBrowsePaths(std::vector<BrowsePath> inputs) override {
    co_return co_await
        CallbackToAwaitable<Status, std::vector<BrowsePathResult>>(
            executor_,
            [this, inputs = std::move(inputs)](auto callback) mutable {
              service_.TranslateBrowsePaths(inputs, std::move(callback));
            });
  }

 private:
  const AnyExecutor executor_;
  ViewService& service_;
};

class CallbackToCoroutineNodeManagementServiceAdapter final
    : public CoroutineNodeManagementService {
 public:
  CallbackToCoroutineNodeManagementServiceAdapter(
      AnyExecutor executor,
      NodeManagementService& service)
      : executor_{std::move(executor)}, service_{service} {}
  CallbackToCoroutineNodeManagementServiceAdapter(
      std::shared_ptr<Executor> executor,
      NodeManagementService& service)
      : CallbackToCoroutineNodeManagementServiceAdapter(
            MakeAnyExecutor(std::move(executor)), service) {}

  Awaitable<std::tuple<Status, std::vector<AddNodesResult>>> AddNodes(
      std::vector<AddNodesItem> inputs) override {
    co_return co_await CallbackToAwaitable<Status, std::vector<AddNodesResult>>(
        executor_,
        [this, inputs = std::move(inputs)](auto callback) mutable {
          service_.AddNodes(inputs, std::move(callback));
        });
  }

  Awaitable<std::tuple<Status, std::vector<StatusCode>>> DeleteNodes(
      std::vector<DeleteNodesItem> inputs) override {
    co_return co_await CallbackToAwaitable<Status, std::vector<StatusCode>>(
        executor_,
        [this, inputs = std::move(inputs)](auto callback) mutable {
          service_.DeleteNodes(inputs, std::move(callback));
        });
  }

  Awaitable<std::tuple<Status, std::vector<StatusCode>>> AddReferences(
      std::vector<AddReferencesItem> inputs) override {
    co_return co_await CallbackToAwaitable<Status, std::vector<StatusCode>>(
        executor_,
        [this, inputs = std::move(inputs)](auto callback) mutable {
          service_.AddReferences(inputs, std::move(callback));
        });
  }

  Awaitable<std::tuple<Status, std::vector<StatusCode>>> DeleteReferences(
      std::vector<DeleteReferencesItem> inputs) override {
    co_return co_await CallbackToAwaitable<Status, std::vector<StatusCode>>(
        executor_,
        [this, inputs = std::move(inputs)](auto callback) mutable {
          service_.DeleteReferences(inputs, std::move(callback));
        });
  }

 private:
  const AnyExecutor executor_;
  NodeManagementService& service_;
};

class PromiseToCoroutineSessionServiceAdapter final
    : public CoroutineSessionService {
 public:
  PromiseToCoroutineSessionServiceAdapter(AnyExecutor executor,
                                          SessionService& service)
      : executor_{std::move(executor)}, service_{service} {}
  PromiseToCoroutineSessionServiceAdapter(std::shared_ptr<Executor> executor,
                                          SessionService& service)
      : PromiseToCoroutineSessionServiceAdapter(
            MakeAnyExecutor(std::move(executor)), service) {}

  Awaitable<void> Connect(SessionConnectParams params) override {
    co_await AwaitPromise(executor_, service_.Connect(std::move(params)));
  }

  Awaitable<void> Reconnect() override {
    co_await AwaitPromise(executor_, service_.Reconnect());
  }

  Awaitable<void> Disconnect() override {
    co_await AwaitPromise(executor_, service_.Disconnect());
  }

  bool IsConnected(base::TimeDelta* ping_delay = nullptr) const override {
    return service_.IsConnected(ping_delay);
  }

  NodeId GetUserId() const override { return service_.GetUserId(); }

  bool HasPrivilege(Privilege privilege) const override {
    return service_.HasPrivilege(privilege);
  }

  std::string GetHostName() const override { return service_.GetHostName(); }

  bool IsScada() const override { return service_.IsScada(); }

  boost::signals2::scoped_connection SubscribeSessionStateChanged(
      const SessionStateChangedCallback& callback) override {
    return service_.SubscribeSessionStateChanged(callback);
  }

  SessionDebugger* GetSessionDebugger() override {
    return service_.GetSessionDebugger();
  }

 private:
  const AnyExecutor executor_;
  SessionService& service_;
};

class CoroutineToCallbackAttributeServiceAdapter final : public AttributeService {
 public:
  CoroutineToCallbackAttributeServiceAdapter(AnyExecutor executor,
                                             CoroutineAttributeService& service)
      : executor_{std::move(executor)}, service_{service} {}
  CoroutineToCallbackAttributeServiceAdapter(std::shared_ptr<Executor> executor,
                                             CoroutineAttributeService& service)
      : CoroutineToCallbackAttributeServiceAdapter(
            MakeAnyExecutor(std::move(executor)), service) {}

  void Read(const ServiceContext& context,
            const std::shared_ptr<const std::vector<ReadValueId>>& inputs,
            const ReadCallback& callback) override {
    CoSpawn(executor_,
            [this, context, inputs, callback]() mutable -> Awaitable<void> {
              try {
                auto [status, results] =
                    co_await service_.Read(context, std::move(inputs));
                callback(std::move(status), std::move(results));
              } catch (...) {
                callback(GetExceptionStatus(std::current_exception()), {});
              }
            });
  }

  void Write(const ServiceContext& context,
             const std::shared_ptr<const std::vector<WriteValue>>& inputs,
             const WriteCallback& callback) override {
    CoSpawn(executor_,
            [this, context, inputs, callback]() mutable -> Awaitable<void> {
              try {
                auto [status, results] =
                    co_await service_.Write(context, std::move(inputs));
                callback(std::move(status), std::move(results));
              } catch (...) {
                auto status = GetExceptionStatus(std::current_exception());
                callback(std::move(status), {});
              }
            });
  }

 private:
  const AnyExecutor executor_;
  CoroutineAttributeService& service_;
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

class CoroutineToCallbackHistoryServiceAdapter final : public HistoryService {
 public:
  CoroutineToCallbackHistoryServiceAdapter(AnyExecutor executor,
                                           CoroutineHistoryService& service)
      : executor_{std::move(executor)}, service_{service} {}
  CoroutineToCallbackHistoryServiceAdapter(std::shared_ptr<Executor> executor,
                                           CoroutineHistoryService& service)
      : CoroutineToCallbackHistoryServiceAdapter(
            MakeAnyExecutor(std::move(executor)), service) {}

  void HistoryReadRaw(const HistoryReadRawDetails& details,
                      const HistoryReadRawCallback& callback) override {
    CoSpawn(executor_,
            [this, details, callback]() mutable -> Awaitable<void> {
              try {
                callback(co_await service_.HistoryReadRaw(details));
              } catch (...) {
                callback(HistoryReadRawResult{
                    .status = GetExceptionStatus(std::current_exception())});
              }
            });
  }

  void HistoryReadEvents(const NodeId& node_id,
                         base::Time from,
                         base::Time to,
                         const EventFilter& filter,
                         const HistoryReadEventsCallback& callback) override {
    CoSpawn(executor_,
            [this, node_id, from, to, filter,
             callback]() mutable -> Awaitable<void> {
              try {
                callback(
                    co_await service_.HistoryReadEvents(node_id, from, to, filter));
              } catch (...) {
                callback(HistoryReadEventsResult{
                    .status = GetExceptionStatus(std::current_exception())});
              }
            });
  }

 private:
  const AnyExecutor executor_;
  CoroutineHistoryService& service_;
};

class CoroutineToCallbackViewServiceAdapter final : public ViewService {
 public:
  CoroutineToCallbackViewServiceAdapter(AnyExecutor executor,
                                        CoroutineViewService& service)
      : executor_{std::move(executor)}, service_{service} {}
  CoroutineToCallbackViewServiceAdapter(std::shared_ptr<Executor> executor,
                                        CoroutineViewService& service)
      : CoroutineToCallbackViewServiceAdapter(
            MakeAnyExecutor(std::move(executor)), service) {}

  void Browse(const ServiceContext& context,
              const std::vector<BrowseDescription>& inputs,
              const BrowseCallback& callback) override {
    CoSpawn(executor_,
            [this, context, inputs, callback]() mutable -> Awaitable<void> {
              try {
                auto [status, results] =
                    co_await service_.Browse(context, inputs);
                callback(std::move(status), std::move(results));
              } catch (...) {
                callback(GetExceptionStatus(std::current_exception()), {});
              }
            });
  }

  void TranslateBrowsePaths(
      const std::vector<BrowsePath>& inputs,
      const TranslateBrowsePathsCallback& callback) override {
    CoSpawn(executor_,
            [this, inputs, callback]() mutable -> Awaitable<void> {
              try {
                auto [status, results] =
                    co_await service_.TranslateBrowsePaths(inputs);
                callback(std::move(status), std::move(results));
              } catch (...) {
                callback(GetExceptionStatus(std::current_exception()), {});
              }
            });
  }

 private:
  const AnyExecutor executor_;
  CoroutineViewService& service_;
};

class CoroutineToCallbackNodeManagementServiceAdapter final
    : public NodeManagementService {
 public:
  CoroutineToCallbackNodeManagementServiceAdapter(
      AnyExecutor executor,
      CoroutineNodeManagementService& service)
      : executor_{std::move(executor)}, service_{service} {}
  CoroutineToCallbackNodeManagementServiceAdapter(
      std::shared_ptr<Executor> executor,
      CoroutineNodeManagementService& service)
      : CoroutineToCallbackNodeManagementServiceAdapter(
            MakeAnyExecutor(std::move(executor)), service) {}

  void AddNodes(const std::vector<AddNodesItem>& inputs,
                const AddNodesCallback& callback) override {
    CoSpawn(executor_,
            [this, inputs, callback]() mutable -> Awaitable<void> {
              try {
                auto [status, results] = co_await service_.AddNodes(inputs);
                callback(std::move(status), std::move(results));
              } catch (...) {
                callback(GetExceptionStatus(std::current_exception()), {});
              }
            });
  }

  void DeleteNodes(const std::vector<DeleteNodesItem>& inputs,
                   const DeleteNodesCallback& callback) override {
    CoSpawn(executor_,
            [this, inputs, callback]() mutable -> Awaitable<void> {
              try {
                auto [status, results] = co_await service_.DeleteNodes(inputs);
                callback(std::move(status), std::move(results));
              } catch (...) {
                callback(GetExceptionStatus(std::current_exception()), {});
              }
            });
  }

  void AddReferences(const std::vector<AddReferencesItem>& inputs,
                     const AddReferencesCallback& callback) override {
    CoSpawn(executor_,
            [this, inputs, callback]() mutable -> Awaitable<void> {
              try {
                auto [status, results] =
                    co_await service_.AddReferences(inputs);
                callback(std::move(status), std::move(results));
              } catch (...) {
                auto status = GetExceptionStatus(std::current_exception());
                callback(std::move(status), {});
              }
            });
  }

  void DeleteReferences(const std::vector<DeleteReferencesItem>& inputs,
                        const DeleteReferencesCallback& callback) override {
    CoSpawn(executor_,
            [this, inputs, callback]() mutable -> Awaitable<void> {
              try {
                auto [status, results] =
                    co_await service_.DeleteReferences(inputs);
                callback(std::move(status), std::move(results));
              } catch (...) {
                auto status = GetExceptionStatus(std::current_exception());
                callback(std::move(status), {});
              }
            });
  }

 private:
  const AnyExecutor executor_;
  CoroutineNodeManagementService& service_;
};

class CoroutineToPromiseSessionServiceAdapter final : public SessionService {
 public:
  CoroutineToPromiseSessionServiceAdapter(std::shared_ptr<Executor> executor,
                                          CoroutineSessionService& service)
      : executor_{std::move(executor)}, service_{service} {}

  promise<void> Connect(const SessionConnectParams& params) override {
    return ToPromise(NetExecutorAdapter{executor_}, service_.Connect(params));
  }

  promise<void> Reconnect() override {
    return ToPromise(NetExecutorAdapter{executor_}, service_.Reconnect());
  }

  promise<void> Disconnect() override {
    return ToPromise(NetExecutorAdapter{executor_}, service_.Disconnect());
  }

  bool IsConnected(base::TimeDelta* ping_delay = nullptr) const override {
    return service_.IsConnected(ping_delay);
  }

  NodeId GetUserId() const override { return service_.GetUserId(); }

  bool HasPrivilege(Privilege privilege) const override {
    return service_.HasPrivilege(privilege);
  }

  std::string GetHostName() const override { return service_.GetHostName(); }

  bool IsScada() const override { return service_.IsScada(); }

  boost::signals2::scoped_connection SubscribeSessionStateChanged(
      const SessionStateChangedCallback& callback) override {
    return service_.SubscribeSessionStateChanged(callback);
  }

  SessionDebugger* GetSessionDebugger() override {
    return service_.GetSessionDebugger();
  }

 private:
  const std::shared_ptr<Executor> executor_;
  CoroutineSessionService& service_;
};

}  // namespace scada
