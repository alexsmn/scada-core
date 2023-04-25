#pragma once

namespace scada {
class AttributeService;
class EventService;
class HistoryService;
class MethodService;
class MonitoredItemService;
class NodeManagementService;
class ViewService;
}  // namespace scada

struct RemoteServices {
  scada::NodeManagementService& node_management_service_;
  scada::AttributeService& attribute_service_;
  scada::MethodService& method_service_;
  scada::ViewService& view_service_;
  scada::MonitoredItemService& monitored_item_service_;
  scada::EventService& event_service_;
  scada::HistoryService& history_service_;
};
