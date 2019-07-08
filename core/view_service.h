#pragma once

#include <functional>
#include <vector>

#include "core/configuration_types.h"
#include "core/debug_util.h"
#include "core/expanded_node_id.h"
#include "core/node_class.h"
#include "core/qualified_name.h"
#include "core/status.h"

namespace scada {

struct ModelChangeEvent;

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
  virtual void OnNodeSemanticsChanged(const NodeId& node_id) = 0;
};

using BrowseCallback =
    std::function<void(Status&& status, std::vector<BrowseResult>&& results)>;
using TranslateBrowsePathsCallback =
    std::function<void(Status&& status,
                       std::vector<BrowsePathResult>&& results)>;

class ViewService {
 public:
  virtual ~ViewService() {}

  virtual void Browse(const std::vector<BrowseDescription>& descriptions,
                      const BrowseCallback& callback) = 0;

  virtual void TranslateBrowsePaths(
      const std::vector<BrowsePath>& browse_paths,
      const TranslateBrowsePathsCallback& callback) = 0;

  virtual void Subscribe(ViewEvents& events) = 0;
  virtual void Unsubscribe(ViewEvents& events) = 0;
};

class LocalViewService {
 public:
  ~LocalViewService() {}

  virtual BrowseResult Browse(const BrowseDescription& node) = 0;
};

}  // namespace scada

inline std::ostream& operator<<(std::ostream& stream,
                                scada::BrowseDirection v) {
  std::string_view name;

  switch (v) {
    case scada::BrowseDirection::Forward:
      name = "Forward";
      break;
    case scada::BrowseDirection::Inverse:
      name = "Inverse";
      break;
    case scada::BrowseDirection::Both:
      name = "Both";
      break;
    default:
      assert(false);
      break;
  }

  return stream << name;
}

inline std::ostream& operator<<(std::ostream& stream,
                                const scada::BrowseDescription& v) {
  return stream << "{node_id: " << v.node_id << ", direction: " << v.direction
                << ", reference_type_id: " << v.reference_type_id
                << ", include_subtypes: " << v.include_subtypes << "}";
}

inline std::ostream& operator<<(std::ostream& stream,
                                const scada::ReferenceDescription& v) {
  return stream << "{reference_type_id: " << v.reference_type_id << ", "
                << "forward: " << v.forward << ", "
                << "node_id: " << v.node_id << "}";
}

inline std::ostream& operator<<(std::ostream& stream,
                                const scada::BrowseResult& v) {
  return stream << "{status_code: " << v.status_code
                << ", references: " << v.references << "}";
}

inline std::ostream& operator<<(std::ostream& stream,
                                const scada::RelativePathElement& v) {
  return stream << "{reference_type_id: " << v.reference_type_id
                << ", inverse: " << v.inverse << ", "
                << ", include_subtypes: " << v.include_subtypes << ", "
                << ", target_name: " << v.target_name << "}";
}

inline std::ostream& operator<<(std::ostream& stream,
                                const scada::BrowsePath& v) {
  return stream << "{node_id: " << v.node_id
                << ", relative_path: " << v.relative_path << "}";
}

inline std::ostream& operator<<(std::ostream& stream,
                                const scada::BrowsePathTarget& v) {
  return stream << "{target_id: " << v.target_id
                << ", remaining_path_index: " << v.remaining_path_index << "}";
}

inline std::ostream& operator<<(std::ostream& stream,
                                const scada::BrowsePathResult& v) {
  return stream << "{status_code: " << v.status_code
                << ", targets: " << v.targets << "}";
}

