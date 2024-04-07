#include "scada/client.h"

#include "scada/node_management_service.h"
#include "scada/service_context.h"
#include "scada/view_service_promises.h"

namespace scada {

client::client() = default;

client::client(const services& services) : services_{services} {}

client client::with_context(const ServiceContext& context) const {
  return client{services_, context};
}

status_promise<void> client::connect(const SessionConnectParams& params) const {
  if (!services_.session_service) {
    return scada::MakeRejectedStatusPromise(
        scada::StatusCode::Bad_Disconnected);
  }

  return services_.session_service->Connect(params);
}

status_promise<void> client::disconnect() const {
  if (!services_.session_service) {
    return scada::MakeRejectedStatusPromise(
        scada::StatusCode::Bad_Disconnected);
  }

  return services_.session_service->Disconnect();
}

status_promise<
    std::vector<scada::StatusOr<std::vector<scada::ReferenceDescription>>>>
client::browse(const std::vector<scada::BrowseDescription>& inputs) const {
  if (!services_.view_service) {
    return scada::MakeRejectedStatusPromise<std::vector<
        scada::StatusOr<std::vector<scada::ReferenceDescription>>>>(
        scada::StatusCode::Bad_Disconnected);
  }

  return scada::Browse(*services_.view_service, context_, inputs);
}

status_promise<scada::node> client::add_node(const AddNodesItem& item) const {
  if (!services_.node_management_service) {
    return MakeRejectedStatusPromise<scada::node>(StatusCode::Bad_Disconnected);
  }

  status_promise<scada::NodeId> status_promise;
  services_.node_management_service->AddNodes(
      {item}, [status_promise](Status&& status,
                               std::vector<AddNodesResult>&& results) mutable {
        if (!status) {
          scada::RejectStatusPromise(status_promise, std::move(status));
          return;
        }
        assert(results.size() == 1);
        const auto& [status_code, node_id] = results[0];
        if (scada::IsBad(status_code)) {
          scada::RejectStatusPromise(status_promise, status_code);
          return;
        }
        assert(!node_id.is_null());
        status_promise.resolve(node_id);
      });

  return status_promise.then(
      [*this](const scada::NodeId& node_id) { return node(node_id); });
}

status_promise<void> client::acknowledge_events(
    std::vector<EventId> event_ids,
    DateTime acknowledge_time) const {
  assert(!event_ids.empty());
  return server_node().call(scada::id::AcknowledgeableConditionType_Acknowledge,
                            std::move(event_ids), acknowledge_time);
}

}  // namespace scada
