#pragma once

#include "base/any_executor.h"
#include "base/awaitable.h"
#include "base/any_executor.h"
#include "scada/attribute_service.h"
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
                            ServiceContext context);

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
    AnyExecutor executor,
    AttributeService& service,
    ServiceContext context,
    std::shared_ptr<const std::vector<ReadValueId>> inputs) {
  (void)executor;
  co_return co_await service.Read(std::move(context), std::move(inputs));
}

inline Awaitable<StatusOr<std::vector<BrowseResult>>> BrowseAsync(
    AnyExecutor executor,
    ViewService& service,
    ServiceContext context,
    std::vector<BrowseDescription> inputs) {
  (void)executor;
  co_return co_await service.Browse(std::move(context), std::move(inputs));
}

inline Awaitable<StatusOr<std::vector<BrowsePathResult>>>
TranslateBrowsePathsAsync(AnyExecutor executor,
                          ViewService& service,
                          std::vector<BrowsePath> inputs) {
  (void)executor;
  co_return co_await service.TranslateBrowsePaths(std::move(inputs));
}

inline Awaitable<StatusOr<std::vector<StatusCode>>> WriteAsync(
    AnyExecutor executor,
    AttributeService& service,
    const ServiceContext& context,
    std::shared_ptr<const std::vector<WriteValue>> inputs) {
  (void)executor;
  co_return co_await service.Write(context, std::move(inputs));
}

inline Awaitable<Status> CallAsync(AnyExecutor executor,
                                   MethodService& service,
                                   NodeId node_id,
                                   NodeId method_id,
                                   std::vector<Variant> arguments,
                                   ServiceContext context) {
  co_return co_await service.Call(std::move(node_id), std::move(method_id),
                                  std::move(arguments), std::move(context));
}

inline Awaitable<HistoryReadRawResult> HistoryReadRawAsync(
    AnyExecutor executor,
    HistoryService& service,
    HistoryReadRawDetails details) {
  (void)executor;
  co_return co_await service.HistoryReadRaw(std::move(details));
}

inline Awaitable<HistoryReadEventsResult> HistoryReadEventsAsync(
    AnyExecutor executor,
    HistoryService& service,
    NodeId node_id,
    base::Time from,
    base::Time to,
    EventFilter filter) {
  (void)executor;
  co_return co_await service.HistoryReadEvents(
      std::move(node_id), from, to, std::move(filter));
}

inline Awaitable<StatusOr<std::vector<AddNodesResult>>> AddNodesAsync(
    AnyExecutor executor,
    NodeManagementService& service,
    std::vector<AddNodesItem> inputs) {
  (void)executor;
  co_return co_await service.AddNodes(ServiceContext{}, std::move(inputs));
}

inline Awaitable<StatusOr<std::vector<StatusCode>>> DeleteNodesAsync(
    AnyExecutor executor,
    NodeManagementService& service,
    std::vector<DeleteNodesItem> inputs) {
  (void)executor;
  co_return co_await service.DeleteNodes(ServiceContext{}, std::move(inputs));
}

inline Awaitable<StatusOr<std::vector<StatusCode>>>
AddReferencesAsync(AnyExecutor executor,
                   NodeManagementService& service,
                   std::vector<AddReferencesItem> inputs) {
  (void)executor;
  co_return co_await service.AddReferences(ServiceContext{}, std::move(inputs));
}

inline Awaitable<StatusOr<std::vector<StatusCode>>>
DeleteReferencesAsync(AnyExecutor executor,
                      NodeManagementService& service,
                      std::vector<DeleteReferencesItem> inputs) {
  (void)executor;
  co_return co_await service.DeleteReferences(ServiceContext{},
                                              std::move(inputs));
}

}  // namespace scada
