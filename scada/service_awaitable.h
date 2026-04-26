#pragma once

#include "base/any_executor.h"
#include "base/awaitable.h"
#include "base/executor_conversions.h"
#include "scada/attribute_service.h"
#include "scada/callback_awaitable.h"
#include "scada/history_service.h"
#include "scada/method_service.h"
#include "scada/node_management_service.h"
#include "scada/service_context.h"
#include "scada/status_or.h"
#include "scada/view_service.h"

namespace scada {

Awaitable<StatusOr<std::vector<DataValue>>> ReadAsync(
    AnyExecutor executor,
    AttributeService& service,
    ServiceContext context,
    std::shared_ptr<const std::vector<ReadValueId>> inputs);

Awaitable<StatusOr<std::vector<BrowseResult>>> BrowseAsync(
    AnyExecutor executor,
    ViewService& service,
    ServiceContext context,
    std::vector<BrowseDescription> inputs);

Awaitable<StatusOr<std::vector<BrowsePathResult>>>
TranslateBrowsePathsAsync(AnyExecutor executor,
                          ViewService& service,
                          std::vector<BrowsePath> inputs);

Awaitable<StatusOr<std::vector<StatusCode>>> WriteAsync(
    AnyExecutor executor,
    AttributeService& service,
    const ServiceContext& context,
    std::shared_ptr<const std::vector<WriteValue>> inputs);

Awaitable<Status> CallAsync(AnyExecutor executor,
                            MethodService& service,
                            NodeId node_id,
                            NodeId method_id,
                            std::vector<Variant> arguments,
                            NodeId user_id);

Awaitable<HistoryReadRawResult> HistoryReadRawAsync(
    AnyExecutor executor,
    HistoryService& service,
    HistoryReadRawDetails details);

Awaitable<HistoryReadEventsResult> HistoryReadEventsAsync(
    AnyExecutor executor,
    HistoryService& service,
    NodeId node_id,
    base::Time from,
    base::Time to,
    EventFilter filter);

Awaitable<StatusOr<std::vector<AddNodesResult>>> AddNodesAsync(
    AnyExecutor executor,
    NodeManagementService& service,
    std::vector<AddNodesItem> inputs);

Awaitable<StatusOr<std::vector<StatusCode>>> DeleteNodesAsync(
    AnyExecutor executor,
    NodeManagementService& service,
    std::vector<DeleteNodesItem> inputs);

Awaitable<StatusOr<std::vector<StatusCode>>> AddReferencesAsync(
    AnyExecutor executor,
    NodeManagementService& service,
    std::vector<AddReferencesItem> inputs);

Awaitable<StatusOr<std::vector<StatusCode>>> DeleteReferencesAsync(
    AnyExecutor executor,
    NodeManagementService& service,
    std::vector<DeleteReferencesItem> inputs);

inline Awaitable<StatusOr<std::vector<DataValue>>> ReadAsync(
    const std::shared_ptr<Executor>& executor,
    AttributeService& service,
    ServiceContext context,
    std::shared_ptr<const std::vector<ReadValueId>> inputs) {
  co_return co_await ReadAsync(MakeAnyExecutor(executor), service,
                               std::move(context), std::move(inputs));
}

inline Awaitable<StatusOr<std::vector<BrowseResult>>> BrowseAsync(
    const std::shared_ptr<Executor>& executor,
    ViewService& service,
    ServiceContext context,
    std::vector<BrowseDescription> inputs) {
  co_return co_await BrowseAsync(MakeAnyExecutor(executor), service,
                                 std::move(context), std::move(inputs));
}

inline Awaitable<StatusOr<std::vector<BrowsePathResult>>>
TranslateBrowsePathsAsync(const std::shared_ptr<Executor>& executor,
                          ViewService& service,
                          std::vector<BrowsePath> inputs) {
  co_return co_await TranslateBrowsePathsAsync(MakeAnyExecutor(executor),
                                               service, std::move(inputs));
}

inline Awaitable<StatusOr<std::vector<StatusCode>>> WriteAsync(
    const std::shared_ptr<Executor>& executor,
    AttributeService& service,
    const ServiceContext& context,
    std::shared_ptr<const std::vector<WriteValue>> inputs) {
  co_return co_await WriteAsync(MakeAnyExecutor(executor), service, context,
                                std::move(inputs));
}

inline Awaitable<Status> CallAsync(const std::shared_ptr<Executor>& executor,
                                   MethodService& service,
                                   NodeId node_id,
                                   NodeId method_id,
                                   std::vector<Variant> arguments,
                                   NodeId user_id) {
  co_return co_await CallAsync(MakeAnyExecutor(executor), service,
                               std::move(node_id), std::move(method_id),
                               std::move(arguments), std::move(user_id));
}

inline Awaitable<HistoryReadRawResult> HistoryReadRawAsync(
    const std::shared_ptr<Executor>& executor,
    HistoryService& service,
    HistoryReadRawDetails details) {
  co_return co_await HistoryReadRawAsync(MakeAnyExecutor(executor), service,
                                         std::move(details));
}

inline Awaitable<HistoryReadEventsResult> HistoryReadEventsAsync(
    const std::shared_ptr<Executor>& executor,
    HistoryService& service,
    NodeId node_id,
    base::Time from,
    base::Time to,
    EventFilter filter) {
  co_return co_await HistoryReadEventsAsync(
      MakeAnyExecutor(executor), service, std::move(node_id), from, to,
      std::move(filter));
}

inline Awaitable<StatusOr<std::vector<AddNodesResult>>> AddNodesAsync(
    const std::shared_ptr<Executor>& executor,
    NodeManagementService& service,
    std::vector<AddNodesItem> inputs) {
  co_return co_await AddNodesAsync(MakeAnyExecutor(executor), service,
                                   std::move(inputs));
}

inline Awaitable<StatusOr<std::vector<StatusCode>>> DeleteNodesAsync(
    const std::shared_ptr<Executor>& executor,
    NodeManagementService& service,
    std::vector<DeleteNodesItem> inputs) {
  co_return co_await DeleteNodesAsync(MakeAnyExecutor(executor), service,
                                      std::move(inputs));
}

inline Awaitable<StatusOr<std::vector<StatusCode>>>
AddReferencesAsync(const std::shared_ptr<Executor>& executor,
                   NodeManagementService& service,
                   std::vector<AddReferencesItem> inputs) {
  co_return co_await AddReferencesAsync(MakeAnyExecutor(executor), service,
                                        std::move(inputs));
}

inline Awaitable<StatusOr<std::vector<StatusCode>>>
DeleteReferencesAsync(const std::shared_ptr<Executor>& executor,
                      NodeManagementService& service,
                      std::vector<DeleteReferencesItem> inputs) {
  co_return co_await DeleteReferencesAsync(MakeAnyExecutor(executor), service,
                                           std::move(inputs));
}

inline Awaitable<StatusOr<std::vector<DataValue>>> ReadAsync(
    AnyExecutor executor,
    AttributeService& service,
    ServiceContext context,
    std::shared_ptr<const std::vector<ReadValueId>> inputs) {
  co_return co_await AwaitStatusOrCallback<std::vector<DataValue>>(
      std::move(executor),
      [&service, context = std::move(context),
       inputs = std::move(inputs)](auto callback) mutable {
        service.Read(context, inputs, std::move(callback));
      });
}

inline Awaitable<StatusOr<std::vector<BrowseResult>>> BrowseAsync(
    AnyExecutor executor,
    ViewService& service,
    ServiceContext context,
    std::vector<BrowseDescription> inputs) {
  co_return co_await AwaitStatusOrCallback<std::vector<BrowseResult>>(
      std::move(executor),
      [&service, context = std::move(context),
       inputs = std::move(inputs)](auto callback) mutable {
        service.Browse(context, inputs, std::move(callback));
      });
}

inline Awaitable<StatusOr<std::vector<BrowsePathResult>>>
TranslateBrowsePathsAsync(AnyExecutor executor,
                          ViewService& service,
                          std::vector<BrowsePath> inputs) {
  co_return co_await AwaitStatusOrCallback<std::vector<BrowsePathResult>>(
      std::move(executor),
      [&service, inputs = std::move(inputs)](auto callback) mutable {
        service.TranslateBrowsePaths(inputs, std::move(callback));
      });
}

inline Awaitable<StatusOr<std::vector<StatusCode>>> WriteAsync(
    AnyExecutor executor,
    AttributeService& service,
    const ServiceContext& context,
    std::shared_ptr<const std::vector<WriteValue>> inputs) {
  co_return co_await AwaitStatusOrCallback<std::vector<StatusCode>>(
      std::move(executor),
      [&service, &context, inputs = std::move(inputs)](auto callback) mutable {
        service.Write(context, inputs, std::move(callback));
      });
}

inline Awaitable<Status> CallAsync(AnyExecutor executor,
                                   MethodService& service,
                                   NodeId node_id,
                                   NodeId method_id,
                                   std::vector<Variant> arguments,
                                   NodeId user_id) {
  co_return co_await AwaitStatusCallback(
      std::move(executor),
      [&service, node_id = std::move(node_id),
       method_id = std::move(method_id), arguments = std::move(arguments),
       user_id = std::move(user_id)](auto callback) mutable {
        service.Call(node_id, method_id, arguments, user_id,
                     std::move(callback));
      });
}

inline Awaitable<HistoryReadRawResult> HistoryReadRawAsync(
    AnyExecutor executor,
    HistoryService& service,
    HistoryReadRawDetails details) {
  co_return co_await AwaitCallbackValue<HistoryReadRawResult>(
      std::move(executor),
      [&service, details = std::move(details)](auto callback) mutable {
        service.HistoryReadRaw(details, std::move(callback));
      });
}

inline Awaitable<HistoryReadEventsResult> HistoryReadEventsAsync(
    AnyExecutor executor,
    HistoryService& service,
    NodeId node_id,
    base::Time from,
    base::Time to,
    EventFilter filter) {
  co_return co_await AwaitCallbackValue<HistoryReadEventsResult>(
      std::move(executor),
      [&service, node_id = std::move(node_id), from, to,
       filter = std::move(filter)](auto callback) mutable {
        service.HistoryReadEvents(node_id, from, to, filter,
                                  std::move(callback));
      });
}

inline Awaitable<StatusOr<std::vector<AddNodesResult>>> AddNodesAsync(
    AnyExecutor executor,
    NodeManagementService& service,
    std::vector<AddNodesItem> inputs) {
  co_return co_await AwaitStatusOrCallback<std::vector<AddNodesResult>>(
      std::move(executor),
      [&service, inputs = std::move(inputs)](auto callback) mutable {
        service.AddNodes(inputs, std::move(callback));
      });
}

inline Awaitable<StatusOr<std::vector<StatusCode>>> DeleteNodesAsync(
    AnyExecutor executor,
    NodeManagementService& service,
    std::vector<DeleteNodesItem> inputs) {
  co_return co_await AwaitStatusOrCallback<std::vector<StatusCode>>(
      std::move(executor),
      [&service, inputs = std::move(inputs)](auto callback) mutable {
        service.DeleteNodes(inputs, std::move(callback));
      });
}

inline Awaitable<StatusOr<std::vector<StatusCode>>>
AddReferencesAsync(AnyExecutor executor,
                   NodeManagementService& service,
                   std::vector<AddReferencesItem> inputs) {
  co_return co_await AwaitStatusOrCallback<std::vector<StatusCode>>(
      std::move(executor),
      [&service, inputs = std::move(inputs)](auto callback) mutable {
        service.AddReferences(inputs, std::move(callback));
      });
}

inline Awaitable<StatusOr<std::vector<StatusCode>>>
DeleteReferencesAsync(AnyExecutor executor,
                      NodeManagementService& service,
                      std::vector<DeleteReferencesItem> inputs) {
  co_return co_await AwaitStatusOrCallback<std::vector<StatusCode>>(
      std::move(executor),
      [&service, inputs = std::move(inputs)](auto callback) mutable {
        service.DeleteReferences(inputs, std::move(callback));
      });
}

}  // namespace scada
