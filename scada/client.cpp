#include "scada/client.h"

#include "scada/node_management_service.h"

namespace scada {

// client

promise<> client::connect(const SessionConnectParams& params) const {
  if (!services_.session_service) {
    return scada::MakeRejectedStatusPromise(
        scada::StatusCode::Bad_Disconnected);
  }

  return services_.session_service->Connect(params);
}

promise<> client::disconnect() const {
  if (!services_.session_service) {
    return scada::MakeRejectedStatusPromise(
        scada::StatusCode::Bad_Disconnected);
  }

  return services_.session_service->Disconnect();
}

promise<scada::node> client::add_node(const AddNodesItem& item) {
  if (!services_.node_management_service) {
    return MakeRejectedStatusPromise<scada::node>(StatusCode::Bad_Disconnected);
  }

  promise<scada::NodeId> promise;
  services_.node_management_service->AddNodes(
      {item}, [promise](Status&& status,
                        std::vector<AddNodesResult>&& results) mutable {
        if (!status) {
          scada::RejectStatusPromise(promise, std::move(status));
          return;
        }
        assert(results.size() == 1);
        const auto& [status_code, node_id] = results[0];
        if (scada::IsBad(status_code)) {
          scada::RejectStatusPromise(promise, status_code);
          return;
        }
        assert(!node_id.is_null());
        promise.resolve(node_id);
      });

  return promise.then(
      [*this](const scada::NodeId& node_id) { return node(node_id); });
}

promise<> client::acknowledge_events(std::vector<EventAcknowledgeId> event_ids,
                                     DateTime acknowledge_time) const {
  assert(!event_ids.empty());
  return server_node().call(scada::id::AcknowledgeableConditionType_Acknowledge,
                            std::move(event_ids), acknowledge_time);
}

}  // namespace scada