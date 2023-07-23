#include "remote/subscription_proxy.h"

#include "base/cancelation.h"
#include "scada/monitored_item_service.h"
#include "remote/message_sender.h"
#include "remote/monitored_item_proxy.h"
#include "remote/protocol.h"
#include "remote/protocol_utils.h"

// SubscriptionProxy

SubscriptionProxy::SubscriptionProxy(const SubscriptionParams& params) {}

SubscriptionProxy::~SubscriptionProxy() {
  if (state_ == State::CREATED) {
    for (auto& monitored_item : CollectMonitoredItems())
      monitored_item->OnChannelClosed();

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

  auto monitored_item = std::make_shared<MonitoredItemProxy>(value_id, params);

  monitored_items_.emplace_back(monitored_item);

  if (state_ == State::CREATED) {
    assert(sender_);
    monitored_item->OnChannelOpened(*this, *sender_, subscription_id_);
  }

  return monitored_item;
}

void SubscriptionProxy::AddMonitoredItemDataObserver(
    MonitoredItemId monitored_item_id,
    MonitoredItemProxy& item) {
  assert(monitored_item_ids_.find(monitored_item_id) ==
         monitored_item_ids_.end());

  monitored_item_ids_[monitored_item_id] = &item;
}

void SubscriptionProxy::RemoveMonitoredItemDataObserver(
    MonitoredItemId monitored_item_id) {
  assert(monitored_item_ids_.find(monitored_item_id) !=
         monitored_item_ids_.end());

  monitored_item_ids_.erase(monitored_item_id);
}

void SubscriptionProxy::OnDataChange(int monitored_item_id,
                                     const scada::DataValue& data_value) {
  auto i = monitored_item_ids_.find(monitored_item_id);
  if (i != monitored_item_ids_.end())
    i->second->OnDataChange(data_value);
}

void SubscriptionProxy::OnEvent(int monitored_item_id,
                                const scada::Status& status,
                                const std::any& event) {
  auto i = monitored_item_ids_.find(monitored_item_id);
  if (i != monitored_item_ids_.end())
    i->second->OnEvent(status, event);
}

void SubscriptionProxy::OnChannelOpened(MessageSender& sender) {
  assert(!sender_);
  assert(state_ == State::DELETED);

  sender_ = &sender;
  state_ = State::CREATING;

  protocol::Request request;
  auto& create_subscription = *request.mutable_create_subscription();
  create_subscription;

  sender_->Request(
      request,
      BindCancelation(
          weak_from_this(), [this](const protocol::Response& response) {
            OnCreateSubscriptionResult(
                ConvertTo<scada::Status>(response.status()),
                response.create_subscription_result().subscription_id());
          }));
}

void SubscriptionProxy::OnChannelClosed() {
  assert(state_ != State::DELETED);
  assert(sender_);

  sender_ = nullptr;
  state_ = State::DELETED;
  subscription_id_ = 0;

  for (auto& monitored_item : CollectMonitoredItems())
    monitored_item->OnChannelClosed();
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

  for (auto& monitored_item : CollectMonitoredItems())
    monitored_item->OnChannelOpened(*this, *sender_, subscription_id_);
}

std::vector<std::shared_ptr<MonitoredItemProxy>>
SubscriptionProxy::CollectMonitoredItems() {
  std::vector<std::shared_ptr<MonitoredItemProxy>> monitored_items;
  monitored_items.reserve(monitored_items_.size());
  auto erase_from = std::remove_if(
      monitored_items_.begin(), monitored_items_.end(),
      [&monitored_items](
          const std::weak_ptr<MonitoredItemProxy>& weak_monitored_item) {
        auto monitored_item = weak_monitored_item.lock();
        if (!monitored_item)
          return true;
        monitored_items.emplace_back(std::move(monitored_item));
        return false;
      });
  monitored_items_.erase(erase_from, monitored_items_.end());
  return monitored_items;
}
