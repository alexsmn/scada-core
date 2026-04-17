#pragma once

#include "base/awaitable.h"
#include "base/callback_awaitable.h"
#include "base/executor.h"
#include "scada/attribute_service.h"
#include "scada/history_service.h"
#include "scada/method_service.h"
#include "scada/node_management_service.h"
#include "scada/service_context.h"

namespace scada {

inline Awaitable<std::tuple<Status, std::vector<DataValue>>> ReadAsync(
    const std::shared_ptr<Executor>& executor,
    AttributeService& service,
    ServiceContext context,
    std::shared_ptr<const std::vector<ReadValueId>> inputs) {
  co_return co_await CallbackToAwaitable<Status, std::vector<DataValue>>(
      executor, [&service, context = std::move(context), inputs = std::move(inputs)](
                    auto callback) mutable {
        service.Read(context, inputs, std::move(callback));
      });
}

inline Awaitable<std::tuple<Status, std::vector<StatusCode>>> WriteAsync(
    const std::shared_ptr<Executor>& executor,
    AttributeService& service,
    const ServiceContext& context,
    std::shared_ptr<const std::vector<WriteValue>> inputs) {
  co_return co_await CallbackToAwaitable<Status, std::vector<StatusCode>>(
      executor, [&service, &context, inputs = std::move(inputs)](
                    auto callback) mutable {
        service.Write(context, inputs, std::move(callback));
      });
}

inline Awaitable<Status> CallAsync(const std::shared_ptr<Executor>& executor,
                                   MethodService& service,
                                   NodeId node_id,
                                   NodeId method_id,
                                   std::vector<Variant> arguments,
                                   NodeId user_id) {
  auto [status] = co_await CallbackToAwaitable<Status>(
      executor, [&service, node_id = std::move(node_id),
                 method_id = std::move(method_id),
                 arguments = std::move(arguments),
                 user_id = std::move(user_id)](auto callback) mutable {
        service.Call(node_id, method_id, arguments, user_id,
                     std::move(callback));
      });
  co_return std::move(status);
}

inline Awaitable<HistoryReadRawResult> HistoryReadRawAsync(
    const std::shared_ptr<Executor>& executor,
    HistoryService& service,
    HistoryReadRawDetails details) {
  auto [result] = co_await CallbackToAwaitable<HistoryReadRawResult>(
      executor, [&service, details = std::move(details)](auto callback) mutable {
        service.HistoryReadRaw(details, std::move(callback));
      });
  co_return std::move(result);
}

inline Awaitable<HistoryReadEventsResult> HistoryReadEventsAsync(
    const std::shared_ptr<Executor>& executor,
    HistoryService& service,
    NodeId node_id,
    base::Time from,
    base::Time to,
    EventFilter filter) {
  auto [result] = co_await CallbackToAwaitable<HistoryReadEventsResult>(
      executor,
      [&service, node_id = std::move(node_id), from, to,
       filter = std::move(filter)](auto callback) mutable {
        service.HistoryReadEvents(node_id, from, to, filter,
                                  std::move(callback));
      });
  co_return std::move(result);
}

inline Awaitable<std::tuple<Status, std::vector<AddNodesResult>>> AddNodesAsync(
    const std::shared_ptr<Executor>& executor,
    NodeManagementService& service,
    std::vector<AddNodesItem> inputs) {
  co_return co_await CallbackToAwaitable<Status, std::vector<AddNodesResult>>(
      executor, [&service, inputs = std::move(inputs)](auto callback) mutable {
        service.AddNodes(inputs, std::move(callback));
      });
}

inline Awaitable<std::tuple<Status, std::vector<StatusCode>>> DeleteNodesAsync(
    const std::shared_ptr<Executor>& executor,
    NodeManagementService& service,
    std::vector<DeleteNodesItem> inputs) {
  co_return co_await CallbackToAwaitable<Status, std::vector<StatusCode>>(
      executor, [&service, inputs = std::move(inputs)](auto callback) mutable {
        service.DeleteNodes(inputs, std::move(callback));
      });
}

inline Awaitable<std::tuple<Status, std::vector<StatusCode>>>
AddReferencesAsync(const std::shared_ptr<Executor>& executor,
                   NodeManagementService& service,
                   std::vector<AddReferencesItem> inputs) {
  co_return co_await CallbackToAwaitable<Status, std::vector<StatusCode>>(
      executor, [&service, inputs = std::move(inputs)](auto callback) mutable {
        service.AddReferences(inputs, std::move(callback));
      });
}

inline Awaitable<std::tuple<Status, std::vector<StatusCode>>>
DeleteReferencesAsync(const std::shared_ptr<Executor>& executor,
                      NodeManagementService& service,
                      std::vector<DeleteReferencesItem> inputs) {
  co_return co_await CallbackToAwaitable<Status, std::vector<StatusCode>>(
      executor, [&service, inputs = std::move(inputs)](auto callback) mutable {
        service.DeleteReferences(inputs, std::move(callback));
      });
}

}  // namespace scada
