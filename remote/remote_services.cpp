#include "core/data_services_factory.h"

#include "base/strings/string_util.h"
#include "remote/session_proxy.h"
#include "remote/view_service_proxy.h"

bool CreateRemoteServices(const DataServicesContext& context,
                          DataServices& services) {
  auto session_proxy = std::make_shared<SessionProxy>(SessionProxyContext{
      context.executor, context.transport_factory, context.service_log_params});

  auto raw_services = session_proxy->services();

  services = {
      .session_service_ =
          std::shared_ptr<scada::SessionService>{session_proxy,
                                                 raw_services.session_service},
      .view_service_ =
          std::shared_ptr<scada::ViewService>{session_proxy,
                                              raw_services.view_service},
      .node_management_service_ =
          std::shared_ptr<scada::NodeManagementService>{
              session_proxy, raw_services.node_management_service},
      .history_service_ =
          std::shared_ptr<scada::HistoryService>{session_proxy,
                                                 raw_services.history_service},
      .attribute_service_ =
          std::shared_ptr<scada::AttributeService>{
              session_proxy, raw_services.attribute_service},
      .method_service_ =
          std::shared_ptr<scada::MethodService>{session_proxy,
                                                raw_services.method_service},
      .monitored_item_service_ = std::shared_ptr<scada::MonitoredItemService>{
          session_proxy, raw_services.monitored_item_service}};

  return true;
}
