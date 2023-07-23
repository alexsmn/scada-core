#pragma once

#include "scada/attribute_ids.h"
#include "scada/status.h"
#include "remote/monitored_item_router.h"
#include "remote/subscription.h"

#include <memory>
#include <unordered_map>
#include <vector>

namespace scada {
class Event;
class MonitoredItem;
struct MonitoringParameters;
struct ReadValueId;
}  // namespace scada

namespace protocol {
class Response;
}

class MessageSender;
class MonitoredItemProxy;

class SubscriptionProxy
    : private MonitoredItemRouter,
      public std::enable_shared_from_this<SubscriptionProxy> {
 public:
  explicit SubscriptionProxy(const SubscriptionParams& params);
  ~SubscriptionProxy();

  std::shared_ptr<scada::MonitoredItem> CreateMonitoredItem(
      const scada::ReadValueId& value_id,
      const scada::MonitoringParameters& params);

  void OnChannelOpened(MessageSender& sender);
  void OnChannelClosed();
  void OnDataChange(int monitored_item_id, const scada::DataValue& data_value);
  void OnEvent(int monitored_item_id,
               const scada::Status& status,
               const std::any& event);

 private:
  void OnCreateSubscriptionResult(const scada::Status& status,
                                  int subscription_id);

  std::vector<std::shared_ptr<MonitoredItemProxy>> CollectMonitoredItems();

  // MonitoredItemRouter
  virtual void AddMonitoredItemDataObserver(MonitoredItemId monitored_item_id,
                                            MonitoredItemProxy& item) override;
  virtual void RemoveMonitoredItemDataObserver(
      MonitoredItemId monitored_item_id) override;

  MessageSender* sender_ = nullptr;

  std::vector<std::weak_ptr<MonitoredItemProxy>> monitored_items_;

  std::unordered_map<MonitoredItemId, MonitoredItemProxy*> monitored_item_ids_;

  enum class State { DELETED, CREATING, CREATED };
  State state_ = State::DELETED;

  int subscription_id_ = 0;

  friend class MonitoredItemProxy;
};
