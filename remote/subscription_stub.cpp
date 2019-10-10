#include "remote/subscription_stub.h"

#include "core/monitored_item.h"
#include "core/monitored_item_service.h"
#include "remote/message_sender.h"
#include "remote/protocol.h"
#include "remote/protocol_utils.h"
#include "remote/subscription.h"

SubscriptionStub::SubscriptionStub(
    MessageSender& sender,
    scada::MonitoredItemService& monitored_item_service,
    int subscription_id,
    const SubscriptionParams& params)
    : sender_{sender},
      monitored_item_service_{monitored_item_service},
      subscription_id_(subscription_id) {}

SubscriptionStub::~SubscriptionStub() {}

void SubscriptionStub::OnCreateMonitoredItem(
    int request_id,
    const scada::ReadValueId& value_id,
    const scada::MonitoringParameters& params) {
  LOG_INFO(logger_) << "Create monitored item"
                    << LOG_TAG("RequestId", request_id)
                    << LOG_TAG("NodeId", ToString(value_id.node_id))
                    << LOG_TAG("AttributeId", ToString(value_id.attribute_id));

  auto channel = monitored_item_service_.CreateMonitoredItem(value_id, params);
  if (!channel) {
    LOG_WARNING(logger_) << "Can't create monitored item"
                         << LOG_TAG("RequestId", request_id)
                         << LOG_TAG("NodeId", ToString(value_id.node_id))
                         << LOG_TAG("AttributeId",
                                    ToString(value_id.attribute_id));

    protocol::Message message;
    auto& response = *message.add_responses();
    response.set_request_id(request_id);
    auto& create_monitored_item_result =
        *response.mutable_create_monitored_item_result();
    Convert(scada::StatusCode::Bad, *response.mutable_status());
    sender_.Send(message);
    return;
  }

  auto monitored_item_id = next_monitored_item_id_++;
  assert(channels_.find(monitored_item_id) == channels_.end());

  LOG_INFO(logger_) << "Create monitored item complete"
                    << LOG_TAG("RequestId", request_id)
                    << LOG_TAG("NodeId", ToString(value_id.node_id))
                    << LOG_TAG("AttributeId", ToString(value_id.attribute_id))
                    << LOG_TAG("MonitoredItemId", monitored_item_id);

  auto channel_ptr = channel.get();
  channels_[monitored_item_id] = std::move(channel);

  {
    protocol::Message message;
    auto& response = *message.add_responses();
    response.set_request_id(request_id);
    auto& create_monitored_item_result =
        *response.mutable_create_monitored_item_result();
    Convert(scada::StatusCode::Good, *response.mutable_status());
    create_monitored_item_result.set_monitored_item_id(monitored_item_id);
    sender_.Send(message);
  }

  if (value_id.attribute_id == scada::AttributeId::EventNotifier) {
    channel_ptr->SubscribeEvents(
        [this, monitored_item_id](const scada::Status& status,
                                  const std::any& event) {
          OnEvent(monitored_item_id, status, event);
        });

  } else {
    channel_ptr->SubscribeData(
        [this, monitored_item_id](const scada::DataValue& data_value) {
          OnDataChange(monitored_item_id, data_value);
        });
  }
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
  Convert(scada::StatusCode::Good, *response.mutable_status());
  sender_.Send(message);
}

void SubscriptionStub::OnDataChange(MonitoredItemId monitored_item_id,
                                    const scada::DataValue& data_value) {
  auto i = channels_.find(monitored_item_id);
  if (i == channels_.end())
    return;

  if (data_value.qualifier.failed()) {
    LOG_WARNING(logger_) << "Monitored item failed"
                         << LOG_TAG("MonitoredItemId", monitored_item_id);

    channels_.erase(i);
  }

  protocol::Message message;
  auto& notification = *message.add_notifications();
  notification.set_subscription_id(subscription_id_);
  auto& data_change = *notification.add_data_changes();
  data_change.set_monitored_item_id(monitored_item_id);
  Convert(data_value, *data_change.mutable_data_value());
  sender_.Send(message);
}

void SubscriptionStub::OnEvent(MonitoredItemId monitored_item_id,
                               const scada::Status& status,
                               const std::any& event) {
  auto i = channels_.find(monitored_item_id);
  if (i == channels_.end())
    return;

  if (!status) {
    LOG_WARNING(logger_) << "Monitored item failed"
                         << LOG_TAG("MonitoredItemId", monitored_item_id)
                         << LOG_TAG("Status", ToString(status));

    channels_.erase(i);
  }

  protocol::Message message;

  auto& notification = *message.add_notifications();
  notification.set_subscription_id(subscription_id_);
  notification.set_monitored_item_id(monitored_item_id);
  if (!status)
    Convert(status, *notification.mutable_status());

  if (auto* e = std::any_cast<scada::Event>(&event))
    Convert(*e, *notification.add_events());
  else if (auto* e = std::any_cast<scada::ModelChangeEvent>(&event))
    Convert(*e, *notification.add_model_change());
  else if (auto* e = std::any_cast<scada::SemanticChangeEvent>(&event))
    Convert(e->node_id, *notification.add_semantics_changed_node_id());

  sender_.Send(message);
}
