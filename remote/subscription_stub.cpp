#include "remote/subscription_stub.h"

#include "base/executor.h"
#include "core/monitored_item.h"
#include "core/monitored_item_service.h"
#include "remote/message_sender.h"
#include "remote/protocol.h"
#include "remote/protocol_utils.h"
#include "remote/subscription.h"

#include "core/debug_util-inl.h"

SubscriptionStub::SubscriptionStub(
    std::shared_ptr<Executor> executor,
    std::weak_ptr<MessageSender> sender,
    scada::MonitoredItemService& monitored_item_service,
    int subscription_id,
    const SubscriptionParams& params)
    : executor_{std::move(executor)},
      sender_{std::move(sender)},
      monitored_item_service_{monitored_item_service},
      subscription_id_(subscription_id) {
  LOG_BIND_TAG(logger_, "SubscriptionId", subscription_id_);
}

SubscriptionStub::~SubscriptionStub() {}

void SubscriptionStub::OnCreateMonitoredItem(
    int request_id,
    const scada::ReadValueId& read_value_id,
    const scada::MonitoringParameters& params) {
  LOG_INFO(logger_) << "Create monitored item"
                    << LOG_TAG("RequestId", request_id)
                    << LOG_TAG("NodeId", ToString(read_value_id.node_id))
                    << LOG_TAG("AttributeId",
                               ToString(read_value_id.attribute_id));

  auto channel =
      monitored_item_service_.CreateMonitoredItem(read_value_id, params);
  if (!channel) {
    LOG_WARNING(logger_) << "Can't create monitored item"
                         << LOG_TAG("RequestId", request_id)
                         << LOG_TAG("NodeId", ToString(read_value_id.node_id))
                         << LOG_TAG("AttributeId",
                                    ToString(read_value_id.attribute_id));

    protocol::Message message;
    auto& response = *message.add_responses();
    response.set_request_id(request_id);
    auto& create_monitored_item_result =
        *response.mutable_create_monitored_item_result();
    create_monitored_item_result;
    Convert(scada::Status{scada::StatusCode::Bad}, *response.mutable_status());

    if (auto locked_sender = sender_.lock())
      locked_sender->Send(message);
    return;
  }

  auto monitored_item_id = next_monitored_item_id_++;
  assert(channels_.find(monitored_item_id) == channels_.end());

  LOG_INFO(logger_) << "Create monitored item complete"
                    << LOG_TAG("RequestId", request_id)
                    << LOG_TAG("NodeId", ToString(read_value_id.node_id))
                    << LOG_TAG("AttributeId",
                               ToString(read_value_id.attribute_id))
                    << LOG_TAG("MonitoredItemId", monitored_item_id);

  auto channel_ptr = channel.get();
  channels_[monitored_item_id] = std::move(channel);

  {
    protocol::Message message;
    auto& response = *message.add_responses();
    response.set_request_id(request_id);
    auto& create_monitored_item_result =
        *response.mutable_create_monitored_item_result();
    create_monitored_item_result;
    Convert(scada::Status{scada::StatusCode::Good}, *response.mutable_status());
    create_monitored_item_result.set_monitored_item_id(monitored_item_id);

    if (auto locked_sender = sender_.lock())
      locked_sender->Send(message);
  }

  if (read_value_id.attribute_id == scada::AttributeId::Value) {
    channel_ptr->set_data_change_handler(BindExecutor(
        executor_, [weak_this = weak_from_this(),
                    monitored_item_id](const scada::DataValue& data_value) {
          if (auto ptr = weak_this.lock())
            ptr->OnDataChange(monitored_item_id, data_value);
        }));

  } else if (read_value_id.attribute_id == scada::AttributeId::EventNotifier) {
    channel_ptr->set_event_handler(BindExecutor(
        executor_, [weak_this = weak_from_this(), monitored_item_id](
                       const scada::Status& status, const std::any& event) {
          if (auto ptr = weak_this.lock())
            ptr->OnEvent(monitored_item_id, status.code(), event);
        }));
  }

  channel_ptr->Subscribe();
}

void SubscriptionStub::OnDeleteMonitoredItem(int request_id,
                                             int monitored_item_id) {
  LOG_INFO(logger_) << "Delete monitored item"
                    << LOG_TAG("RequestId", request_id)
                    << LOG_TAG("MonitoredItemId", monitored_item_id);

  channels_.erase(monitored_item_id);

  protocol::Message message;
  auto& response = *message.add_responses();
  response.set_request_id(request_id);
  Convert(scada::Status{scada::StatusCode::Good}, *response.mutable_status());

  if (auto locked_sender = sender_.lock())
    locked_sender->Send(message);
}

void SubscriptionStub::OnDataChange(MonitoredItemId monitored_item_id,
                                    const scada::DataValue& data_value) {
  auto i = channels_.find(monitored_item_id);
  if (i == channels_.end())
    return;

  if (data_value.qualifier.failed()) {
    LOG_WARNING(logger_) << "Monitored item failed"
                         << LOG_TAG("MonitoredItemId", monitored_item_id)
                         << LOG_TAG("StatusCode",
                                    ToString(data_value.status_code));

    channels_.erase(i);
  }

  protocol::Message message;
  auto& notification = *message.add_notifications();
  notification.set_subscription_id(subscription_id_);
  notification.set_monitored_item_id(monitored_item_id);
  auto& data_change = *notification.add_data_changes();
  data_change.set_monitored_item_id(monitored_item_id);
  Convert(data_value, *data_change.mutable_data_value());

  if (auto locked_sender = sender_.lock())
    locked_sender->Send(message);
}

void SubscriptionStub::OnEvent(MonitoredItemId monitored_item_id,
                               scada::StatusCode status_code,
                               const std::any& event) {
  auto i = channels_.find(monitored_item_id);
  if (i == channels_.end())
    return;

  if (scada::IsBad(status_code)) {
    LOG_WARNING(logger_) << "Monitored item failed"
                         << LOG_TAG("MonitoredItemId", monitored_item_id)
                         << LOG_TAG("StatusCode", ToString(status_code));

    channels_.erase(i);
  }

  protocol::Message message;

  auto& notification = *message.add_notifications();
  notification.set_subscription_id(subscription_id_);
  notification.set_monitored_item_id(monitored_item_id);
  if (status_code != scada::StatusCode::Good)
    Convert(status_code, *notification.mutable_status_code());

  if (auto* system_event = std::any_cast<scada::Event>(&event)) {
    Convert(*system_event, *notification.add_events());
  } else if (auto* model_change_event =
                 std::any_cast<scada::ModelChangeEvent>(&event)) {
    Convert(*model_change_event, *notification.add_model_change());
  } else if (auto* semantic_change_event =
                 std::any_cast<scada::SemanticChangeEvent>(&event)) {
    Convert(semantic_change_event->node_id,
            *notification.add_semantics_changed_node_id());
  }

  if (auto locked_sender = sender_.lock())
    locked_sender->Send(message);
}
