#include "remote/monitored_item_proxy.h"

#include "remote/message_sender.h"
#include "remote/protocol.h"
#include "remote/protocol_utils.h"
#include "remote/subscription_proxy.h"

// MonitoredItemProxy

MonitoredItemProxy::MonitoredItemProxy(SubscriptionProxy& subscription,
                                       scada::ReadValueId read_value_id,
                                       scada::MonitoringParameters params)
    : subscription_{&subscription},
      read_value_id_{std::move(read_value_id)},
      params_{std::move(params)},
      cancelation_(std::make_shared<bool>(false)) {}

MonitoredItemProxy::~MonitoredItemProxy() {
  if (state_ == State::CREATED)
    DeleteStub();

  if (subscription_)
    subscription_->RemoveMonitoredItem(*this);
}

void MonitoredItemProxy::Subscribe(scada::MonitoredItemHandler handler) {
  assert(!handler_.has_value());
  assert(subscription_);
  assert(state_ == State::DELETED);

  handler_ = std::move(handler);

  subscription_->AddMonitoredItem(*this);

  if (!current_data_.is_null())
    NotifyDataChange(current_data_);
}

void MonitoredItemProxy::CreateStub() {
  assert(subscription_);
  assert(state_ == State::DELETED);

  state_ = State::CREATING;

  protocol::Request request;
  auto& create_monitored_item = *request.mutable_create_monitored_item();
  create_monitored_item.set_subscription_id(subscription_->subscription_id_);
  Convert(read_value_id_.node_id, *create_monitored_item.mutable_node_id());
  create_monitored_item.set_attribute_id(
      static_cast<protocol::AttributeId>(read_value_id_.attribute_id));
  if (!params_.is_null())
    Convert(params_, *create_monitored_item.mutable_monitoring_parameters());

  std::weak_ptr<bool> cancelation = cancelation_;
  subscription_->sender_->Request(
      request, [this, cancelation](const protocol::Response& response) {
        if (cancelation.expired())
          return;
        int monitored_item_id = 0;
        if (response.has_create_monitored_item_result()) {
          auto& m = response.create_monitored_item_result();
          monitored_item_id = m.monitored_item_id();
        }
        OnCreateMonitoredItemResult(ConvertTo<scada::Status>(response.status()),
                                    monitored_item_id);
      });
}

void MonitoredItemProxy::DeleteStub() {
  assert(subscription_);
  assert(state_ == State::CREATING || state_ == State::CREATED);

  state_ = State::DELETED;

  auto monitored_item_id = monitored_item_id_;
  subscription_->monitored_item_ids_.erase(monitored_item_id_);
  monitored_item_id_ = 0;

  protocol::Message message;
  auto& request = *message.add_requests();
  request.set_request_id(0);
  auto& delete_monitored_item = *request.mutable_delete_monitored_item();
  delete_monitored_item.set_subscription_id(subscription_->subscription_id_);
  delete_monitored_item.set_monitored_item_id(monitored_item_id);
  subscription_->sender_->Send(message);
}

void MonitoredItemProxy::OnChannelOpened() {
  assert(subscription_);
  assert(state_ == State::DELETED);

  scada::DataValue tvq;
  tvq.server_timestamp = base::Time::Now();
  tvq.qualifier = scada::Qualifier::OFFLINE;
  current_data_ = tvq;
  NotifyDataChange(tvq);

  CreateStub();
}

void MonitoredItemProxy::OnChannelClosed() {
  assert(subscription_);
  assert(state_ != State::DELETED);

  state_ = State::DELETED;

  subscription_->monitored_item_ids_.erase(monitored_item_id_);
  monitored_item_id_ = 0;

  UpdateQualifier(0, scada::Qualifier::OFFLINE);
}

void MonitoredItemProxy::OnCreateMonitoredItemResult(
    const scada::Status& status,
    int monitored_item_id) {
  assert(subscription_);

  if (!status) {
    state_ = State::DELETED;
    subscription_->RemoveMonitoredItem(*this);
    UpdateQualifier(0, scada::Qualifier::FAILED);
    return;
  }

  state_ = State::CREATED;
  monitored_item_id_ = monitored_item_id;
  subscription_->monitored_item_ids_[monitored_item_id_] = this;
}

void MonitoredItemProxy::UpdateAndForwardData(const scada::DataValue& value) {
  assert(subscription_);

  if (value == current_data_)
    return;

  // Update current value only with latest time.
  if (IsUpdate(current_data_, value))
    current_data_ = value;

  // But any data shall be notified to delegate.
  NotifyDataChange(value);
}

void MonitoredItemProxy::Delete() {
  assert(subscription_);
  assert(state_ == State::DELETED);

  subscription_->RemoveMonitoredItem(*this);
  subscription_ = nullptr;
}

void MonitoredItemProxy::UpdateQualifier(unsigned remove, unsigned add) {
  scada::Qualifier new_qualifier = current_data_.qualifier;
  new_qualifier.Update(remove, add);
  if (current_data_.qualifier == new_qualifier)
    return;

  current_data_.qualifier = new_qualifier;

  auto data = current_data_;
  NotifyDataChange(data);
}

void MonitoredItemProxy::NotifyDataChange(const scada::DataValue& data_value) {
  if (!handler_.has_value())
    return;

  auto* data_change_handler = std::get_if<scada::DataChangeHandler>(&*handler_);
  if (data_change_handler)
    (*data_change_handler)(data_value);
}

void MonitoredItemProxy::NotifyEvent(const scada::Status& status,
                                     const std::any& event) {
  if (!handler_.has_value())
    return;

  auto* event_handler = std::get_if<scada::EventHandler>(&*handler_);
  if (event_handler)
    (*event_handler)(status, event);
}
