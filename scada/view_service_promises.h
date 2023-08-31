#pragma once

#include "base/promise.h"
#include "scada/status_or.h"
#include "scada/view_service.h"

namespace scada {

// TODO: Use status promise.
inline promise<BrowseResult> Browse(ViewService& service,
                                    const BrowseDescription& input) {
  promise<BrowseResult> promise;
  Browse(service, input, [promise](scada::BrowseResult&& result) mutable {
    promise.resolve(std::move(result));
  });
  return promise;
}

// TODO: Use status promise.
inline promise<StatusCodeOr<NodeId>> BrowseParentId(
    ViewService& service,
    const scada::NodeId& node_id) {
  return Browse(service,
                scada::BrowseDescription{node_id, BrowseDirection::Inverse,
                                         id::HierarchicalReferences, true})
      .then(
          [](const scada::BrowseResult& result) -> StatusCodeOr<scada::NodeId> {
            if (IsBad(result.status_code)) {
              return result.status_code;
            } else if (result.references.empty()) {
              return scada::NodeId{};
            } else {
              assert(result.references.size() == 1);
              return result.references[0].node_id;
            }
          });
}

}  // namespace scada
