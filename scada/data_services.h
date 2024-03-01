#pragma once

#include "scada/services.h"

#include <memory>

namespace scada {
class NodeManagementService;
class HistoryService;
class AttributeService;
class MethodService;
class MonitoredItemService;
class SessionService;
class ViewService;
}  // namespace scada

// TODO: Replace with `std::shared_ptr<scada::services>`.
struct DataServices {
  std::shared_ptr<scada::SessionService> session_service_;
  std::shared_ptr<scada::ViewService> view_service_;
  std::shared_ptr<scada::NodeManagementService> node_management_service_;
  std::shared_ptr<scada::HistoryService> history_service_;
  std::shared_ptr<scada::AttributeService> attribute_service_;
  std::shared_ptr<scada::MethodService> method_service_;
  std::shared_ptr<scada::MonitoredItemService> monitored_item_service_;
};

namespace scada {

inline services GetServices(DataServices& data_services) {
  return {
      .attribute_service = data_services.attribute_service_.get(),
      .monitored_item_service = data_services.monitored_item_service_.get(),
      .method_service = data_services.method_service_.get(),
      .history_service = data_services.history_service_.get(),
      .view_service = data_services.view_service_.get(),
      .node_management_service = data_services.node_management_service_.get(),
      .session_service = data_services.session_service_.get()};
}

}  // namespace scada
