#pragma once

#include "scada/node_management_service.h"
#include "scada/status_promise.h"

namespace scada {

inline status_promise<std::vector<AddNodesResult>> AddNode(
    NodeManagementService& service,
    const std::vector<AddNodesItem>& inputs) {
  status_promise<std::vector<AddNodesResult>> p;
  service.AddNodes(
      inputs, [p](Status status, std::vector<AddNodesResult> results) mutable {
        if (status) {
          p.resolve(std::move(results));
        } else {
          scada::RejectStatusPromise(p, std::move(status));
        }
      });
  return p;
}

inline status_promise<NodeId> AddNode(NodeManagementService& service,
                                      const AddNodesItem& input) {
  status_promise<NodeId> p;

  service.AddNodes(
      {input}, [p](Status status, std::vector<AddNodesResult> results) mutable {
        if (!status) {
          scada::RejectStatusPromise(p, std::move(status));
          return;
        }

        assert(results.size() == 1);

        auto& [status_code, node_id] = results[0];
        if (IsBad(status_code)) {
          scada::RejectStatusPromise(p, status_code);
          return;
        }

        p.resolve(std::move(node_id));
      });

  return p;
}

}  // namespace scada