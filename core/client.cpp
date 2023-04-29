#include "core/client.h"

namespace scada {

// monitored_item::state

monitored_item::state::state(std::shared_ptr<MonitoredItem> monitored_item)
    : monitored_item_{std::move(monitored_item)} {}

StatusCode monitored_item::state::status_code() const {
  std::lock_guard lock{mutex_};
  return status_code_;
}

DataValue monitored_item::state::data_value() const {
  std::lock_guard lock{mutex_};
  return data_value_;
}

std::any monitored_item::state::last_event() const {
  std::lock_guard lock{mutex_};
  return last_event_;
}

void monitored_item::state::handle_data_change(const DataValue& data_value) {
  // Release monitored item on bad status code.
  if (scada::IsBad(data_value.status_code)) {
    // Only release monitored item outside of the mutex to avoid deadlocks.
    std::shared_ptr<MonitoredItem> monitored_item;
    std::lock_guard lock{mutex_};
    monitored_item = std::move(monitored_item_);
    status_code_ = data_value.status_code;
    data_value_ = data_value;
    return;
  }

  std::lock_guard lock{mutex_};
  status_code_ = data_value.status_code;
  data_value_ = data_value;
}

void monitored_item::state::handle_event(const Status& status,
                                         const std::any& event) {
  // Release monitored item on bad status code.
  if (!status) {
    // Only release monitored item outside of the mutex to avoid deadlocks.
    std::shared_ptr<MonitoredItem> monitored_item;
    std::lock_guard lock{mutex_};
    monitored_item = std::move(monitored_item_);
    status_code_ = status.code();
    last_event_ = event;
    return;
  }

  std::lock_guard lock{mutex_};
  status_code_ = status.code();
  last_event_ = event;
}

// node

promise<DataValue> node::read(AttributeId attribute_id) const {
  if (!services_.attribute_service) {
    return MakeRejectedStatusPromise<DataValue>(StatusCode::Bad_Disconnected);
  }

  return Read(*services_.attribute_service, context_,
              {.node_id = node_id_, .attribute_id = attribute_id})
      .then([](const scada::DataValue& result) {
        return scada::IsBad(result.status_code)
                   ? MakeRejectedStatusPromise<DataValue>(result.status_code)
                   : make_resolved_promise(result);
      });
}

promise<> node::write(AttributeId attribute_id, const Variant& value) const {
  if (!services_.attribute_service) {
    return MakeRejectedStatusPromise(StatusCode::Bad);
  }

  return ToStatusPromise(Write(
      *services_.attribute_service, context_,
      {.node_id = node_id_, .attribute_id = attribute_id, .value = value}));
}

promise<> node::call_packed(const NodeId& method_id,
                            const std::vector<Variant>& arguments) const {
  if (!services_.method_service) {
    return MakeRejectedStatusPromise(StatusCode::Bad_Disconnected);
  }

  // Take `user_id` from `client`.
  return ToStatusPromise(Call(*services_.method_service, node_id_, method_id,
                              arguments, context_->user_id));
}

promise<std::vector<Event>> node::read_event_history(
    const event_history_details& details) const {
  if (!services_.history_service) {
    return MakeRejectedStatusPromise<std::vector<Event>>(
        StatusCode::Bad_Disconnected);
  }

  return HistoryReadEvents(*services_.history_service, node_id_, details.from,
                           details.to, details.filter);
}

// client

promise<> client::acknowledge_events(std::vector<EventAcknowledgeId> event_ids,
                                     DateTime acknowledge_time) const {
  assert(!event_ids.empty());
  return server_node().call(scada::id::AcknowledgeableConditionType_Acknowledge,
                            std::move(event_ids), acknowledge_time);
}

}  // namespace scada