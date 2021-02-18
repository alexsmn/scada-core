#pragma once

#include "base/boost_log.h"
#include "remote/subscription.h"

#include <map>
#include <memory>

namespace scada {
class Event;
class MonitoredItem;
class MonitoredItemService;
struct MonitoringParameters;
struct ReadValueId;
}  // namespace scada

class Executor;
class MessageSender;

class SubscriptionStub : public std::enable_shared_from_this<SubscriptionStub> {
 public:
  SubscriptionStub(std::shared_ptr<Executor> executor,
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

  const std::shared_ptr<Executor> executor_;
  const std::weak_ptr<MessageSender> sender_;
  scada::MonitoredItemService& monitored_item_service_;
  const int subscription_id_;

  BoostLogger logger_{LOG_NAME("SubscriptionStub")};

  MonitoredItemId next_monitored_item_id_ = 1;
  std::map<MonitoredItemId, std::shared_ptr<scada::MonitoredItem>> channels_;
};
