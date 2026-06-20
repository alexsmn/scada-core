#pragma once

#include "base/any_executor.h"
#include "base/boost_log.h"
#include "remote/subscription.h"
#include "scada/legacy_monitored_item_adapter.h"
#include "scada/read_value_id.h"

#include <memory>
#include <unordered_map>

namespace scada {
class MonitoredItem;
class MonitoredItemService;
struct Event;
struct MonitoringParameters;
struct ReadValueId;
}  // namespace scada

class MessageSender;

class SubscriptionStub : public std::enable_shared_from_this<SubscriptionStub> {
 public:
  SubscriptionStub(AnyExecutor executor,
                   std::weak_ptr<MessageSender> sender,
                   scada::MonitoredItemService& monitored_item_service,
                   int subscription_id,
                   const SubscriptionParams& params);
  ~SubscriptionStub();

  void OnCreateMonitoredItem(int request_id,
                             const scada::ReadValueId& read_value_id,
                             const scada::MonitoringParameters& params);
  void OnDeleteMonitoredItem(int request_id, int monitored_item_id);

 private:
  void OnDataChange(MonitoredItemId monitored_item_id,
                    const scada::DataValue& data_value);
  void OnEvent(MonitoredItemId monitored_item_id,
               scada::StatusCode status_code,
               const std::any& event);

  const AnyExecutor executor_;
  const std::weak_ptr<MessageSender> sender_;
  scada::MonitoredItemService& monitored_item_service_;
  // Bridges the service's subscription API to the single-item API this stub
  // exposes to remote clients.
  scada::LegacyMonitoredItemAdapter monitored_item_adapter_{
      executor_, monitored_item_service_};
  const int subscription_id_;

  BoostLogger logger_{LOG_NAME("SubscriptionStub")};

  MonitoredItemId next_monitored_item_id_ = 1;

  struct ItemInfo {
    scada::ReadValueId read_value_id;
    std::shared_ptr<scada::MonitoredItem> monitored_item;
  };

  std::unordered_map<MonitoredItemId, ItemInfo> monitored_items_;
};
