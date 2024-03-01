#include "remote/monitored_item_proxy.h"

#include "base/cancelation.h"
#include "remote/message_sender.h"
#include "remote/monitored_item_router.h"
#include "remote/protocol_utils.h"
#include "remote/subscription_proxy.h"

#include "base/debug_util-inl.h"

// MonitoredItemProxy

MonitoredItemProxy::MonitoredItemProxy(
    const scada::ReadValueId& value_id,
    const scada::MonitoringParameters& params)
    : value_id_{value_id}, params_{params} {
  LOG_BIND_TAG(logger_, "NodeId", ToString(value_id_.node_id));
  LOG_BIND_TAG(logger_, "AttributeId", ToString(value_id_.attribute_id));
}

MonitoredItemProxy::~MonitoredItemProxy() {
  if (state_ == State::CREATED)
    DeleteStub();
}

void MonitoredItemProxy::Subscribe(scada::MonitoredItemHandler handler) {
  assert(!subscribed_);
  assert(state_ == State::DELETED);

  subscribed_ = true;

  if (auto* data_change_handler =
          std::get_if<scada::DataChangeHandler>(&handler)) {
    data_change_handler_ = std::move(*data_change_handler);

    if (!current_data_.is_null())
      data_change_handler_(current_data_);
  } else if (auto* event_handler = std::get_if<scada::EventHandler>(&handler)) {
    event_handler_ = std::move(*event_handler);
  } else {
    assert(false);
  }

  if (channel_opened_)
    CreateStub();
}

void MonitoredItemProxy::CreateStub() {
  assert(subscribed_);
  assert(state_ == State::DELETED);
  assert(sender_);

  LOG_INFO(logger_) << "Create stub"
                    << LOG_TAG("SubscriptionId", subscription_id_);

  state_ = State::CREATING;

  protocol::Request request;
  auto& create_monitored_item = *request.mutable_create_monitored_item();
  create_monitored_item.set_subscription_id(subscription_id_);
  Convert(value_id_.node_id, *create_monitored_item.mutable_node_id());
  create_monitored_item.set_attribute_id(
      static_cast<protocol::AttributeId>(value_id_.attribute_id));
  if (!params_.is_null())
    Convert(params_, *create_monitored_item.mutable_monitoring_parameters());

  sender_->Request(
      request,
      BindCancelation(
          weak_from_this(), [this](const protocol::Response& response) {
            OnCreateMonitoredItemResult(
                ConvertTo<scada::Status>(response.status()),
                response.create_monitored_item_result().monitored_item_id());
          }));
}

void MonitoredItemProxy::DeleteStub() {
  assert(subscribed_);
  assert(router_);
  assert(state_ == State::CREATED);
  assert(sender_);

  LOG_INFO(logger_) << "Delete stub";

  state_ = State::DELETED;

  auto monitored_item_id = monitored_item_id_;
  router_->RemoveMonitoredItemDataObserver(monitored_item_id_);
  monitored_item_id_ = 0;

  protocol::Request request;
  auto& delete_monitored_item = *request.mutable_delete_monitored_item();
  delete_monitored_item.set_subscription_id(subscription_id_);
  delete_monitored_item.set_monitored_item_id(monitored_item_id);
  sender_->Request(request, nullptr);
}

void MonitoredItemProxy::OnChannelOpened(MonitoredItemRouter& router,
                                         MessageSender& sender,
                                         int subscription_id) {
  assert(state_ == State::DELETED);
  assert(!router_);
  assert(!sender_);
  assert(!channel_opened_);

  router_ = &router;
  sender_ = &sender;
  subscription_id_ = subscription_id;
  channel_opened_ = true;

  if (!subscribed_)
    return;

  if (data_change_handler_) {
    auto tvq = current_data_;
    tvq.server_timestamp = scada::DateTime::Now();
    tvq.qualifier = scada::Qualifier::OFFLINE;
    current_data_ = tvq;
    data_change_handler_(tvq);
  }

  CreateStub();
}

void MonitoredItemProxy::OnChannelClosed() {
  assert(router_);
  assert(sender_);
  assert(channel_opened_);

  if (state_ == State::CREATED)
    router_->RemoveMonitoredItemDataObserver(monitored_item_id_);

  const auto old_state = state_;

  channel_opened_ = false;
  router_ = nullptr;
  sender_ = nullptr;
  state_ = State::DELETED;
  subscription_id_ = 0;
  monitored_item_id_ = 0;

  if (old_state != State::DELETED &&
      value_id_.attribute_id != scada::AttributeId::EventNotifier) {
    UpdateQualifier(current_data_.status_code, 0, scada::Qualifier::OFFLINE);
  }
}

void MonitoredItemProxy::OnCreateMonitoredItemResult(
    const scada::Status& status,
    int monitored_item_id) {
  assert(router_);

  if (!status) {
    LOG_INFO(logger_) << "Create stub error"
                      << LOG_TAG("SubscriptionId", subscription_id_)
                      << LOG_TAG("Status", ToString(status));

    state_ = State::DELETED;
    if (value_id_.attribute_id == scada::AttributeId::EventNotifier)
      event_handler_(status, {});
    else
      UpdateQualifier(status.code(), 0, scada::Qualifier::FAILED);
    return;
  }

  LOG_INFO(logger_) << "Create stub succeeded"
                    << LOG_TAG("SubscriptionId", subscription_id_);

  state_ = State::CREATED;
  monitored_item_id_ = monitored_item_id;
  router_->AddMonitoredItemDataObserver(monitored_item_id_, *this);
}

void MonitoredItemProxy::OnDataChange(scada::DataValue value) {
  assert(router_);
  assert(subscribed_);
  assert(data_change_handler_);
  assert(value_id_.attribute_id != scada::AttributeId::EventNotifier);

  if (state_ != State::CREATED) {
    LOG_WARNING(logger_) << "Unexpected data message"
                         << LOG_TAG("SubscriptionId", subscription_id_)
                         << LOG_TAG("StatusCode", ToString(value.status_code));
    return;
  }

  if (value == current_data_)
    return;

  if (scada::IsBad(value.status_code)) {
    LOG_WARNING(logger_) << "Bad status received"
                         << LOG_TAG("SubscriptionId", subscription_id_)
                         << LOG_TAG("StatusCode", ToString(value.status_code));

    state_ = State::DELETED;
    router_->RemoveMonitoredItemDataObserver(monitored_item_id_);
    monitored_item_id_ = 0;
  }

  // Update current value only with latest time.
  if (IsUpdate(current_data_, value))
    current_data_ = value;

  // But any data shall be notified to delegate.
  data_change_handler_(value);
}

void MonitoredItemProxy::OnEvent(scada::Status status, std::any event) {
  assert(router_);
  assert(subscribed_);
  assert(event_handler_);
  assert(value_id_.attribute_id == scada::AttributeId::EventNotifier);

  if (state_ != State::CREATED) {
    LOG_WARNING(logger_) << "Unexpected data message"
                         << LOG_TAG("SubscriptionId", subscription_id_)
                         << LOG_TAG("Status", ToString(status));
    return;
  }

  if (!status) {
    LOG_WARNING(logger_) << "Bad status received"
                         << LOG_TAG("SubscriptionId", subscription_id_)
                         << LOG_TAG("Status", ToString(status));

    state_ = State::DELETED;
    router_->RemoveMonitoredItemDataObserver(monitored_item_id_);
  }

  event_handler_(std::move(status), std::move(event));
}

void MonitoredItemProxy::UpdateQualifier(scada::StatusCode status_code,
                                         unsigned remove,
                                         unsigned add) {
  assert(value_id_.attribute_id != scada::AttributeId::EventNotifier);

  if (!subscribed_)
    return;

  assert(data_change_handler_);

  scada::Qualifier new_qualifier = current_data_.qualifier;
  new_qualifier.Update(remove, add);

  if (current_data_.status_code == status_code &&
      current_data_.qualifier == new_qualifier)
    return;

  current_data_.status_code = status_code;
  current_data_.qualifier = new_qualifier;

  auto data = current_data_;
  data_change_handler_(data);
}
