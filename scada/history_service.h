#pragma once

#include "base/any_executor.h"
#include "base/awaitable.h"
#include "base/executor_conversions.h"
#include "scada/callback_awaitable.h"
#include "scada/event.h"
#include "scada/history_types.h"
#include "scada/status_exception.h"

#include <memory>
#include <utility>

namespace scada {

class HistoryService {
 public:
  virtual ~HistoryService() {}

  virtual void HistoryReadRaw(const HistoryReadRawDetails& details,
                              const HistoryReadRawCallback& callback) = 0;

  virtual void HistoryReadEvents(const NodeId& node_id,
                                 base::Time from,
                                 base::Time to,
                                 const EventFilter& filter,
                                 const HistoryReadEventsCallback& callback) = 0;
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
    co_return co_await AwaitCallbackValue<HistoryReadRawResult>(
        executor_,
        [this, details = std::move(details)](auto callback) mutable {
          service_.HistoryReadRaw(details, std::move(callback));
        });
  }

  Awaitable<HistoryReadEventsResult> HistoryReadEvents(
      NodeId node_id,
      base::Time from,
      base::Time to,
      EventFilter filter) override {
    co_return co_await AwaitCallbackValue<HistoryReadEventsResult>(
        executor_,
        [this, node_id = std::move(node_id), from, to,
         filter = std::move(filter)](auto callback) mutable {
          service_.HistoryReadEvents(node_id, from, to, filter,
                                     std::move(callback));
        });
  }

 private:
  const AnyExecutor executor_;
  HistoryService& service_;
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
                callback(co_await service_.HistoryReadEvents(node_id, from, to,
                                                             filter));
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

}  // namespace scada
