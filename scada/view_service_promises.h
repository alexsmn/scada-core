#pragma once

#include "base/promise.h"
#include "scada/status_or.h"
#include "scada/view_service.h"

namespace scada {

inline promise<BrowseResult> Browse(ViewService& service,
                                    const BrowseDescription& input) {
  promise<BrowseResult> promise;
  Browse(service, input, [promise](scada::BrowseResult&& result) mutable {
    promise.resolve(std::move(result));
  });
  return promise;
}

inline promise<StatusCodeOr<NodeId>> BrowseParentId(
    ViewService& service,
    const scada::NodeId& node_id) {
  return Browse(service,
                scada::BrowseDescription{node_id, BrowseDirection::Inverse,
                                         id::HierarchicalReferences, true})
      .then([](const scada::BrowseResult& result) {
        if (IsBad(result.status_code))
          return MakeStatusCodeOr<scada::NodeId>(result.status_code);
        if (result.references.empty())
          return MakeStatusCodeOr(scada::NodeId{});
        assert(result.references.size() == 1);
        return MakeStatusCodeOr(result.references[0].node_id);
      });
}

}  // namespace scada
