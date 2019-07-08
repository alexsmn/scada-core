#pragma once

#include <functional>
#include <vector>

#include "core/configuration_types.h"
#include "core/debug_util.h"
#include "core/model_change_event.h"
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

struct BrowseNode {
  NodeId parent_id;
  NodeId reference_type_id;
  NodeId node_id;
  NodeClass node_class;
  NodeId type_id;
  QualifiedName browse_name;
  LocalizedText display_name;
  NodeId data_type_id;
  Variant value;
};

struct RelativePathElement {
  NodeId reference_type_id;
  QualifiedName target_name;
};

using RelativePath = std::vector<RelativePathElement>;

class ViewEvents {
 public:
  virtual ~ViewEvents() {}

  virtual void OnModelChanged(const ModelChangeEvent& event) = 0;
  virtual void OnNodeSemanticsChanged(const NodeId& node_id) = 0;
};

using BrowseCallback =
    std::function<void(Status&& status, std::vector<BrowseResult>&& results)>;
using TranslateBrowsePathCallback =
    std::function<void(Status&& status,
                       std::vector<NodeId>&& target_ids,
                       size_t remaining_path_index)>;

class ViewService {
 public:
  virtual ~ViewService() {}

  virtual void Browse(const std::vector<BrowseDescription>& descriptions,
                      const BrowseCallback& callback) = 0;

  virtual void TranslateBrowsePath(
      const NodeId& starting_node_id,
      const RelativePath& relative_path,
      const TranslateBrowsePathCallback& callback) = 0;

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
