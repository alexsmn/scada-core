#pragma once

#include "base/boost_log.h"
#include "core/monitored_item_service.h"
#include "remote/subscription.h"

class MessageSender;
class MonitoredItemRouter;

class MonitoredItemProxy
    : public scada::MonitoredItem,
      public std::enable_shared_from_this<MonitoredItemProxy> {
 public:
  MonitoredItemProxy(scada::ReadValueId value_id,
                     scada::MonitoringParameters params);
  ~MonitoredItemProxy();

  void OnChannelOpened(MonitoredItemRouter& router,
                       MessageSender& sender,
                       int subscription_id);
  void OnChannelClosed();
  void OnDataChange(scada::DataValue value);
  void OnEvent(scada::Status status, std::any event);

  // scada::MonitoredItem
  virtual void Subscribe(scada::MonitoredItemHandler handler) override;

 private:
  void CreateStub();
  void DeleteStub();

  void OnCreateMonitoredItemResult(const scada::Status& status,
                                   int monitored_item_id);

  void UpdateQualifier(scada::StatusCode status_code,
                       unsigned remove,
                       unsigned add);

  const scada::ReadValueId value_id_;
  const scada::MonitoringParameters params_;

  BoostLogger logger_{LOG_NAME("MonitoredItemProxy")};

  MonitoredItemRouter* router_ = nullptr;
  MessageSender* sender_ = nullptr;
  int subscription_id_ = 0;

  MonitoredItemId monitored_item_id_ = 0;

  enum class State { DELETED, CREATING, CREATED };
  State state_ = State::DELETED;

  scada::DataChangeHandler data_change_handler_;
  scada::EventHandler event_handler_;
  bool subscribed_ = false;
  bool channel_opened_ = false;

  scada::DataValue current_data_;
};
