#include "core/client.h"

namespace scada {

// monitored_item::state

monitored_item::state::state(std::shared_ptr<MonitoredItem> monitored_item)
    : monitored_item_{std::move(monitored_item)} {}

void monitored_item::state::handle_status(scada::StatusCode status_code) {
  // Release monitored item on bad status code.
  if (scada::IsBad(status_code)) {
    // It's safe to reset monitored item outside of the mutex, as it's only
    // updated once.
    monitored_item_.reset();
  }
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