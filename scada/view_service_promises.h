#pragma once

#include "scada/status_or.h"
#include "scada/status_promise.h"
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

inline status_promise<std::vector<BrowsePathResult>> TranslateBrowsePaths(
    ViewService& service,
    const std::vector<BrowsePath>& browse_paths) {
  status_promise<std::vector<BrowsePathResult>> promise;
  service.TranslateBrowsePaths(
      browse_paths, [promise](scada::Status&& status,
                              std::vector<BrowsePathResult>&& results) mutable {
        if (status) {
          promise.resolve(std::move(results));
        } else {
          scada::RejectStatusPromise(promise, std::move(status));
        }
      });
  return promise;
}

inline status_promise<std::vector<BrowsePathTarget>> TranslateBrowsePath(
    ViewService& service,
    const BrowsePath& browse_path) {
  return TranslateBrowsePaths(service, {browse_path})
      .then([](const std::vector<BrowsePathResult>& results) {
        assert(results.size() == 1);
        return results[0];
      })
      .then([](const BrowsePathResult& result) {
        if (scada::IsBad(result.status_code)) {
          throw scada::status_exception{result.status_code};
        }
        return result.targets;
      });
}

inline status_promise<scada::NodeId> TranslateBrowsePathToOneTarget(
    ViewService& service,
    const BrowsePath& browse_path) {
  return TranslateBrowsePath(service, browse_path)
      .then([](const std::vector<scada::BrowsePathTarget>& targets) {
        if (targets.size() != 1) {
          throw scada::status_exception{scada::StatusCode::Bad};
        }
        return targets[0];
      })
      .then([path_size = browse_path.relative_path.size()](
                const scada::BrowsePathTarget& target) {
        if (target.remaining_path_index != path_size) {
          throw scada::status_exception{scada::StatusCode::Bad};
        }
        // TODO: Handle expanded node IDs.
        return target.target_id.node_id();
      });
}

}  // namespace scada
