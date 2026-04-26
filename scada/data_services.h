#pragma once

#include "scada/services.h"

#include <memory>

namespace scada {
class CoroutineAttributeService;
class CoroutineHistoryService;
class CoroutineMethodService;
class CoroutineNodeManagementService;
class CoroutineSessionService;
class CoroutineViewService;
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
            .session_service = session_service_.get()};
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
        .monitored_item_service_ = std::shared_ptr<scada::MonitoredItemService>{
            shared_services, shared_services->monitored_item_service}};
  }

  std::shared_ptr<scada::SessionService> session_service_;
  std::shared_ptr<scada::ViewService> view_service_;
  std::shared_ptr<scada::NodeManagementService> node_management_service_;
  std::shared_ptr<scada::HistoryService> history_service_;
  std::shared_ptr<scada::AttributeService> attribute_service_;
  std::shared_ptr<scada::MethodService> method_service_;
  std::shared_ptr<scada::MonitoredItemService> monitored_item_service_;

  std::shared_ptr<scada::CoroutineSessionService> coroutine_session_service_;
  std::shared_ptr<scada::CoroutineViewService> coroutine_view_service_;
  std::shared_ptr<scada::CoroutineNodeManagementService>
      coroutine_node_management_service_;
  std::shared_ptr<scada::CoroutineHistoryService> coroutine_history_service_;
  std::shared_ptr<scada::CoroutineAttributeService>
      coroutine_attribute_service_;
  std::shared_ptr<scada::CoroutineMethodService> coroutine_method_service_;
};
