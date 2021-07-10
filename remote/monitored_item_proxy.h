#pragma once

#include "core/monitored_item.h"
#include "core/monitored_item_service.h"
#include "remote/subscription.h"

class SubscriptionProxy;

class MonitoredItemProxy : public scada::MonitoredItem {
 public:
  MonitoredItemProxy(SubscriptionProxy& subscription,
                     scada::ReadValueId read_value_id,
                     scada::MonitoringParameters params);
  ~MonitoredItemProxy();

  void OnChannelOpened();
  void OnChannelClosed();

  void UpdateAndForwardData(const scada::DataValue& value);

  void Delete();

  void NotifyDataChange(const scada::DataValue& data_value);
  void NotifyEvent(const scada::Status& status, const std::any& event);

  // scada::MonitoredItem
  virtual void Subscribe(scada::MonitoredItemHandler handler) override;

 private:
  void CreateStub();
  void DeleteStub();

  void OnCreateMonitoredItemResult(const scada::Status& status,
                                   int monitored_item_id);

  void UpdateQualifier(unsigned remove, unsigned add);

  const scada::ReadValueId read_value_id_;
  const scada::MonitoringParameters params_;

  std::optional<scada::MonitoredItemHandler> handler_;

  SubscriptionProxy* subscription_ = nullptr;

  MonitoredItemId monitored_item_id_ = 0;

  enum class State { DELETED, CREATING, CREATED };
  State state_ = State::DELETED;

  scada::DataValue current_data_;

  const std::shared_ptr<bool> cancelation_;
};
