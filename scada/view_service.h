#pragma once

#include "scada/expanded_node_id.h"
#include "scada/node_class.h"
#include "scada/qualified_name.h"
#include "scada/status.h"

#include <functional>
#include <ostream>
#include <vector>

namespace scada {

class ServiceContext;

enum class BrowseDirection { Forward = 0, Inverse = 1, Both = 2 };

struct BrowseDescription {
  bool operator==(const BrowseDescription&) const = default;

  NodeId node_id;
  BrowseDirection direction = BrowseDirection::Both;
  NodeId reference_type_id;
  bool include_subtypes = true;
};

struct ReferenceDescription {
  auto operator<=>(const ReferenceDescription&) const = default;

  NodeId reference_type_id;
  bool forward = true;
  NodeId node_id;
};

using ReferenceDescriptions = std::vector<ReferenceDescription>;

struct BrowseResult {
  StatusCode status_code = StatusCode::Good;
  ReferenceDescriptions references;
};

struct RelativePathElement {
  bool operator==(const RelativePathElement&) const = default;

  NodeId reference_type_id;
  bool inverse = false;
  bool include_subtypes = true;
  QualifiedName target_name;
};

using RelativePath = std::vector<RelativePathElement>;

struct BrowsePath {
  bool operator==(const BrowsePath&) const = default;

  // Must be named |node_id| for generalization.
  NodeId node_id;
  RelativePath relative_path;
};

struct BrowsePathTarget {
  ExpandedNodeId target_id;
  size_t remaining_path_index = 0;
};

// TODO: Replace with `StatusOr`.
struct BrowsePathResult {
  StatusCode status_code = StatusCode::Good;
  std::vector<BrowsePathTarget> targets;
};

using BrowseCallback =
    std::function<void(Status status, std::vector<BrowseResult> results)>;

using TranslateBrowsePathsCallback =
    std::function<void(Status status, std::vector<BrowsePathResult> results)>;

class ViewService {
 public:
  virtual ~ViewService() {}

  virtual void Browse(const ServiceContext& context,
                      const std::vector<BrowseDescription>& inputs,
                      const BrowseCallback& callback) = 0;

  virtual void TranslateBrowsePaths(
      const std::vector<BrowsePath>& inputs,
      const TranslateBrowsePathsCallback& callback) = 0;
};

// Callback = void(const BrowseResult);
template <class Callback>
inline void Browse(ViewService& view_service,
                   const scada::ServiceContext& context,
                   const BrowseDescription& input,
                   Callback&& callback) {
  view_service.Browse(
      context, {input},
      [callback = std::forward<Callback>(callback)](
          Status status, std::vector<BrowseResult> results) mutable {
        if (status)
          callback(std::move(results.front()));
        else
          callback({status.code()});
      });
}

// Callback = void(const BrowsePathResult);
template <class Callback>
inline void TranslateBrowsePath(ViewService& view_service,
                                BrowsePath&& browse_path,
                                Callback&& callback) {
  view_service.TranslateBrowsePaths(
      {std::move(browse_path)},
      [callback = std::forward<Callback>(callback)](
          Status status, std::vector<BrowsePathResult> results) mutable {
        if (status)
          callback(std::move(results.front()));
        else
          callback(BrowsePathResult{status.code()});
      });
}

std::ostream& operator<<(std::ostream& stream, BrowseDirection v);
std::ostream& operator<<(std::ostream& stream, const BrowseDescription& v);
std::ostream& operator<<(std::ostream& stream, const ReferenceDescription& v);
std::ostream& operator<<(std::ostream& stream, const BrowseResult& v);
std::ostream& operator<<(std::ostream& stream, const RelativePathElement& v);
std::ostream& operator<<(std::ostream& stream, const BrowsePath& v);
std::ostream& operator<<(std::ostream& stream, const BrowsePathTarget& v);
std::ostream& operator<<(std::ostream& stream, const BrowsePathResult& v);

}  // namespace scada
