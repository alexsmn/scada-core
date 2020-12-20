#include "remote/event_service_proxy.h"

#include "remote/message_sender.h"
#include "remote/protocol.h"
#include "remote/protocol_utils.h"

EventServiceProxy::EventServiceProxy() {}

EventServiceProxy::~EventServiceProxy() {}

void EventServiceProxy::OnChannelOpened(MessageSender& sender) {
  sender_ = &sender;
}

void EventServiceProxy::OnChannelClosed() {
  sender_ = nullptr;
}

void EventServiceProxy::Acknowledge(base::span<const int> acknowledge_ids,
                                    const scada::NodeId& user_id) {
  if (!sender_) {
    assert(false);
    return;
  }

  protocol::Request request;
  auto& acknowledge = *request.mutable_call()->mutable_acknowledge();
  acknowledge.mutable_acknowledge_id()->Add(acknowledge_ids.begin(),
                                            acknowledge_ids.end());

  sender_->Request(request, nullptr);
}
