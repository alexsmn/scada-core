#pragma once

#include <functional>
#include <vector>

#include "core/configuration_types.h"
#include "core/event.h"
#include "core/node_class.h"
#include "core/node_id.h"
#include "core/qualified_name.h"
#include "core/status.h"

namespace scada {

enum class BrowseDirection {
  Forward = 0,
  Inverse = 1,
  Both = 2,
};

struct BrowseDescription {
  NodeId node_id;
  BrowseDirection direction;
  NodeId reference_type_id;
  bool include_subtypes;
};

struct ReferenceDescription {
  NodeId reference_type_id;
  bool forward;
  NodeId node_id;
};

inline bool operator==(const ReferenceDescription& a,
                       const ReferenceDescription& b) {
  return std::tie(a.reference_type_id, a.forward, a.node_id) ==
         std::tie(b.reference_type_id, b.forward, b.node_id);
}

using ReferenceDescriptions = std::vector<ReferenceDescription>;

struct BrowseResult {
  StatusCode status_code;
  ReferenceDescriptions references;
};

struct RelativePathElement {
  NodeId reference_type_id;
  bool inverse;
  bool include_subtypes;
  QualifiedName target_name;
};

using RelativePath = std::vector<RelativePathElement>;

struct BrowsePath {
  // Must be named |node_id| for generalization.
  scada::NodeId node_id;
  RelativePath relative_path;
};

struct BrowsePathTarget {
  ExpandedNodeId target_id;
  size_t remaining_path_index;
};

struct BrowsePathResult {
  scada::StatusCode status_code;
  std::vector<BrowsePathTarget> targets;
};

class ViewEvents {
 public:
  virtual ~ViewEvents() {}

  virtual void OnModelChanged(const ModelChangeEvent& event) = 0;
  virtual void OnNodeSemanticsChanged(
      const scada::SemanticChangeEvent& event) = 0;
};

using BrowseCallback =
    std::function<void(Status&& status, std::vector<BrowseResult>&& results)>;
using TranslateBrowsePathCallback =
    std::function<void(Status&& status,
                       std::vector<BrowsePathResult>&& results)>;

class ViewService {
 public:
  virtual ~ViewService() {}

  virtual void Browse(const std::vector<BrowseDescription>& descriptions,
                      const BrowseCallback& callback) = 0;

  virtual void TranslateBrowsePaths(
      const std::vector<BrowsePath>& browse_paths,
      const TranslateBrowsePathCallback& callback) = 0;
};

class LocalViewService {
 public:
  ~LocalViewService() {}

  virtual BrowseResult Browse(const BrowseDescription& node) = 0;
};

}  // namespace scada

inline std::ostream& operator<<(std::ostream& stream,
                                const scada::ReferenceDescription& ref) {
  return stream << "{"
                << "reference_type_id: " << ref.reference_type_id << ", "
                << "forward: " << ref.forward << ", "
                << "node_id: " << ref.node_id
                << "}";
}
