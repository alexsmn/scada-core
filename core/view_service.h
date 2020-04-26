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

struct BrowsePath {
  NodeId starting_node_id;
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
