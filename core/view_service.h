#pragma once

#include "base/debug_util.h"
#include "core/expanded_node_id.h"
#include "core/node_class.h"
#include "core/qualified_name.h"
#include "core/status.h"

#include <functional>
#include <vector>

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
  NodeId node_id;
  RelativePath relative_path;
};

struct BrowsePathTarget {
  ExpandedNodeId target_id;
  size_t remaining_path_index;
};

struct BrowsePathResult {
  StatusCode status_code;
  std::vector<BrowsePathTarget> targets;
};

using BrowseCallback =
    std::function<void(Status&& status, std::vector<BrowseResult>&& results)>;
using TranslateBrowsePathsCallback =
    std::function<void(Status&& status,
                       std::vector<BrowsePathResult>&& results)>;

class ViewService {
 public:
  virtual ~ViewService() {}

  virtual void Browse(const std::vector<BrowseDescription>& inputs,
                      const BrowseCallback& callback) = 0;

  virtual void TranslateBrowsePaths(
      const std::vector<BrowsePath>& inputs,
      const TranslateBrowsePathsCallback& callback) = 0;
};

// Callback = void(const scada::BrowseResult);
template <class Callback>
inline void Browse(scada::ViewService& view_service,
                   const scada::BrowseDescription& input,
                   const Callback& callback) {
  view_service.Browse({input}, [callback = std::move(callback)](
                                   scada::Status status,
                                   std::vector<scada::BrowseResult> results) {
    if (status)
      callback(std::move(results.front()));
    else
      callback({status.code()});
  });
}

// Callback = void(const scada::BrowsePathResult);
template <class Callback>
inline void TranslateBrowsePath(scada::ViewService& view_service,
                                const scada::BrowsePath& browse_path,
                                const Callback& callback) {
  view_service.TranslateBrowsePaths(
      {browse_path},
      [callback = std::move(callback)](
          scada::Status status, std::vector<scada::BrowsePathResult> results) {
        if (status)
          callback(std::move(results.front()));
        else
          callback({status.code()});
      });
}

inline std::ostream& operator<<(std::ostream& stream, BrowseDirection v) {
  std::string_view name;

  switch (v) {
    case BrowseDirection::Forward:
      name = "Forward";
      break;
    case BrowseDirection::Inverse:
      name = "Inverse";
      break;
    case BrowseDirection::Both:
      name = "Both";
      break;
    default:
      assert(false);
      break;
  }

  return stream << name;
}

inline bool operator==(const BrowseDescription& a, const BrowseDescription& b) {
  return std::tie(a.node_id, a.direction, a.reference_type_id,
                  a.include_subtypes) == std::tie(b.node_id, b.direction,
                                                  b.reference_type_id,
                                                  b.include_subtypes);
}

inline std::ostream& operator<<(std::ostream& stream,
                                const BrowseDescription& v) {
  return stream << "{node_id: " << v.node_id << ", direction: " << v.direction
                << ", reference_type_id: " << v.reference_type_id
                << ", include_subtypes: " << v.include_subtypes << "}";
}

inline bool operator==(const ReferenceDescription& a,
                       const ReferenceDescription& b) {
  return std::tie(a.reference_type_id, a.forward, a.node_id) ==
         std::tie(b.reference_type_id, b.forward, b.node_id);
}

inline std::ostream& operator<<(std::ostream& stream,
                                const ReferenceDescription& v) {
  return stream << "{reference_type_id: " << v.reference_type_id << ", "
                << "forward: " << v.forward << ", "
                << "node_id: " << v.node_id << "}";
}

inline std::ostream& operator<<(std::ostream& stream, const BrowseResult& v) {
  using ::operator<<;
  return stream << "{status_code: " << v.status_code
                << ", references: " << v.references << "}";
}

inline std::ostream& operator<<(std::ostream& stream,
                                const RelativePathElement& v) {
  return stream << "{reference_type_id: " << v.reference_type_id
                << ", inverse: " << v.inverse << ", "
                << ", include_subtypes: " << v.include_subtypes << ", "
                << ", target_name: " << v.target_name << "}";
}

inline std::ostream& operator<<(std::ostream& stream, const BrowsePath& v) {
  using ::operator<<;
  return stream << "{node_id: " << v.node_id
                << ", relative_path: " << v.relative_path << "}";
}

inline std::ostream& operator<<(std::ostream& stream,
                                const BrowsePathTarget& v) {
  return stream << "{target_id: " << v.target_id
                << ", remaining_path_index: " << v.remaining_path_index << "}";
}

inline std::ostream& operator<<(std::ostream& stream,
                                const BrowsePathResult& v) {
  using ::operator<<;
  return stream << "{status_code: " << v.status_code
                << ", targets: " << v.targets << "}";
}

}  // namespace scada
