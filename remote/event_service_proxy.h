#pragma once

#include "core/event_service.h"

class MessageSender;

class EventServiceProxy : public scada::EventService {
 public:
  EventServiceProxy();
  virtual ~EventServiceProxy();

  void OnChannelOpened(MessageSender& sender);
  void OnChannelClosed();

  // scada::EventService
  virtual void Acknowledge(base::span<const int> acknowledge_ids,
                           const scada::NodeId& user_id) override;

 private:
  MessageSender* sender_ = nullptr;
};
