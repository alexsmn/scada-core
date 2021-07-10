#include "remote/subscription_proxy.h"

#include "core/monitored_item_service.h"
#include "remote/message_sender.h"
#include "remote/monitored_item_proxy.h"
#include "remote/protocol.h"
#include "remote/protocol_utils.h"

// SubscriptionProxy

SubscriptionProxy::SubscriptionProxy(const SubscriptionParams& params) {}

SubscriptionProxy::~SubscriptionProxy() {
  for (auto i = monitored_items_.begin(); i != monitored_items_.end();) {
    auto& monitored_item = **i++;
    monitored_item.Delete();
  }

  assert(monitored_items_.empty());

  if (state_ == State::CREATED) {
    protocol::Message message;
    auto& request = *message.add_requests();
    request.set_request_id(0);
    auto& delete_subscription = *request.mutable_delete_subscription();
    delete_subscription.set_subscription_id(subscription_id_);
    sender_->Send(message);
  }
}

std::shared_ptr<scada::MonitoredItem> SubscriptionProxy::CreateMonitoredItem(
    const scada::ReadValueId& value_id,
    const scada::MonitoringParameters& params) {
  assert(value_id.attribute_id == scada::AttributeId::EventNotifier ||
         !value_id.node_id.is_null());

  return std::make_shared<MonitoredItemProxy>(*this, value_id, params);
}

void SubscriptionProxy::AddMonitoredItem(MonitoredItemProxy& item) {
  monitored_items_.insert(&item);

  if (state_ == State::CREATED)
    item.OnChannelOpened();
}

void SubscriptionProxy::RemoveMonitoredItem(MonitoredItemProxy& item) {
  monitored_items_.erase(&item);
}

void SubscriptionProxy::OnDataChange(int monitored_item_id,
                                     const scada::DataValue& data_value) {
  auto i = monitored_item_ids_.find(monitored_item_id);
  if (i != monitored_item_ids_.end())
    i->second->UpdateAndForwardData(data_value);
}

void SubscriptionProxy::OnEvent(int monitored_item_id,
                                const scada::Status& status,
                                const std::any& event) {
  auto i = monitored_item_ids_.find(monitored_item_id);
  if (i != monitored_item_ids_.end())
    i->second->NotifyEvent(status, event);
}

void SubscriptionProxy::OnChannelOpened(MessageSender& sender) {
  assert(!sender_);
  assert(state_ == State::DELETED);

  sender_ = &sender;
  state_ = State::CREATING;

  protocol::Request request;
  auto& create_subscription = *request.mutable_create_subscription();
  create_subscription;

  sender_->Request(request, [this, weak_ptr = weak_from_this()](
                                const protocol::Response& response) {
    auto ptr = weak_ptr.lock();
    if (!ptr)
      return;

    int subscription_id = 0;
    if (response.has_create_subscription_result()) {
      auto& m = response.create_subscription_result();
      subscription_id = m.subscription_id();
    }

    OnCreateSubscriptionResult(ConvertTo<scada::Status>(response.status()),
                               subscription_id);
  });
}

void SubscriptionProxy::OnChannelClosed() {
  assert(state_ != State::DELETED);
  assert(sender_);

  sender_ = nullptr;

  if (state_ == State::DELETED)
    return;

  state_ = State::DELETED;
  subscription_id_ = 0;

  for (auto* channel : monitored_items_)
    channel->OnChannelClosed();
}

void SubscriptionProxy::OnCreateSubscriptionResult(const scada::Status& status,
                                                   int subscription_id) {
  assert(state_ == State::CREATING);
  assert(sender_);

  if (state_ != State::CREATING)
    return;

  if (!status) {
    state_ = State::DELETED;
    return;
  }

  state_ = State::CREATED;
  subscription_id_ = subscription_id;

  for (auto* channel : monitored_items_)
    channel->OnChannelOpened();
}
