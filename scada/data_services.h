#pragma once

#include "scada/basic_types.h"
#include "scada/services.h"

#include <functional>
#include <memory>
#include <optional>
#include <string_view>

namespace scada {
class HistoryService;
class MethodService;
class NodeManagementService;
class ViewService;
class AttributeService;
}  // namespace scada

// TODO: Move under `scada` namespace.
// TODO: Replace with `std::shared_ptr<scada::services>`.
struct DataServices {
  scada::services as_services() const {
    return {.attribute_service = attribute_service_.get(),
            .monitored_item_service = monitored_item_service_.get(),
            .method_service = method_service_.get(),
            .history_service = history_service_.get(),
            .view_service = view_service_.get(),
            .node_management_service = node_management_service_.get(),
            .session_service = session_service_.get(),
            .monitored_item_executor = monitored_item_executor_};
  }

  std::shared_ptr<const scada::services> CreateSharedServices() const {
    struct Holder {
      DataServices data_services;
      scada::services services{data_services.as_services()};
    };

    auto holder = std::make_shared<Holder>(*this);
    return std::shared_ptr<const scada::services>(holder, &holder->services);
  }

  // The `services` are owned by the caller and must outlast the returned
  // instance.
  static DataServices FromUnownedServices(const scada::services& services) {
    // Create a fake pointer.
    return FromSharedServices(std::make_shared<scada::services>(services));
  }

  static DataServices FromSharedServices(
      const std::shared_ptr<const scada::services>& shared_services) {
    return {
        .session_service_ =
            std::shared_ptr<scada::SessionService>{
                shared_services, shared_services->session_service},
        .view_service_ =
            std::shared_ptr<scada::ViewService>{shared_services,
                                                shared_services->view_service},
        .node_management_service_ =
            std::shared_ptr<scada::NodeManagementService>{
                shared_services, shared_services->node_management_service},
        .history_service_ =
            std::shared_ptr<scada::HistoryService>{
                shared_services, shared_services->history_service},
        .attribute_service_ =
            std::shared_ptr<scada::AttributeService>{
                shared_services, shared_services->attribute_service},
        .method_service_ =
            std::shared_ptr<scada::MethodService>{
                shared_services, shared_services->method_service},
        .monitored_item_service_ =
            std::shared_ptr<scada::MonitoredItemService>{
                shared_services, shared_services->monitored_item_service},
        .monitored_item_executor_ = shared_services->monitored_item_executor};
  }

  std::shared_ptr<scada::SessionService> session_service_;
  std::shared_ptr<scada::ViewService> view_service_;
  std::shared_ptr<scada::NodeManagementService> node_management_service_;
  std::shared_ptr<scada::HistoryService> history_service_;
  std::shared_ptr<scada::AttributeService> attribute_service_;
  std::shared_ptr<scada::MethodService> method_service_;
  std::shared_ptr<scada::MonitoredItemService> monitored_item_service_;

  // Mirrors `scada::services::monitored_item_executor`: required by nodes that
  // flow into the client `monitored_item::subscribe` path (it drives the
  // `LegacyMonitoredItemAdapter` backing single monitored items). Must be
  // carried here so `as_services()` round-trips do not silently drop it.
  AnyExecutor monitored_item_executor_;

  // This connection's NamespaceIndex for a namespace URI, so that a NodeId
  // stored as text in the self-contained "nsu=" form (scada/node_id_string.h)
  // names the same node whatever index the server gives its namespace. A
  // backend that remaps a server's namespaces onto the client's own index space
  // sets it, and returns an index for every URI, appending one the connection
  // has not seen: a node named in a namespace the server does not publish then
  // fails at the server as unknown rather than landing in another namespace.
  // Empty for a backend whose indexes are the client's canonical ones. Not
  // carried by `as_services()`, which holds service pointers only.
  std::function<std::optional<scada::NamespaceIndex>(std::string_view uri)>
      namespace_index_for_uri_;
};
