#pragma once

namespace scada {

class AttributeService;
class MethodService;
class MonitoredItemService;
class NodeManagementService;
class HistoryService;
class SessionService;
class ViewService;

// TODO: Sort services by names.
struct services {
  AttributeService* attribute_service = nullptr;
  MonitoredItemService* monitored_item_service = nullptr;
  MethodService* method_service = nullptr;
  HistoryService* history_service = nullptr;
  ViewService* view_service = nullptr;
  NodeManagementService* node_management_service = nullptr;
  SessionService* session_service = nullptr;
};

}  // namespace scada
