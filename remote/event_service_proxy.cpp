#include "remote/event_service_proxy.h"

#include "remote/message_sender.h"
#include "remote/protocol.h"

EventServiceProxy::EventServiceProxy() {}

EventServiceProxy::~EventServiceProxy() {}

void EventServiceProxy::OnChannelOpened(MessageSender& sender) {
  sender_ = &sender;
}

void EventServiceProxy::OnChannelClosed() {
  sender_ = nullptr;
}

void EventServiceProxy::Acknowledge(
    base::span<const scada::EventAcknowledgeId> acknowledge_ids,
    scada::DateTime acknowledge_time,
    const scada::NodeId& user_id) {
  if (!sender_) {
    assert(false);
    return;
  }

  protocol::Request request;
  auto& acknowledge = *request.mutable_call()->mutable_acknowledge();
  // TODO: Write acknowledge time to the request.
  acknowledge.mutable_acknowledge_id()->Add(acknowledge_ids.begin(),
                                            acknowledge_ids.end());

  sender_->Request(request, nullptr);
}
