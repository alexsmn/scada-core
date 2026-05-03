#pragma once

#include "base/any_executor.h"
#include "base/awaitable.h"
#include "base/any_executor.h"
#include "scada/callback_awaitable.h"
#include "scada/expanded_node_id.h"
#include "scada/node_class.h"
#include "scada/qualified_name.h"
#include "scada/service_context.h"
#include "scada/status.h"
#include "scada/status_exception.h"
#include "scada/status_or.h"

#include <functional>
#include <memory>
#include <ostream>
#include <utility>
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
  bool operator==(const BrowseResult&) const = default;

  StatusCode status_code = StatusCode::Good;
  ByteString continuation_point;
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

class ViewService {
 public:
  virtual ~ViewService() = default;

  virtual Awaitable<StatusOr<std::vector<BrowseResult>>> Browse(
      ServiceContext context,
      std::vector<BrowseDescription> inputs) = 0;

  virtual Awaitable<StatusOr<std::vector<BrowsePathResult>>>
  TranslateBrowsePaths(std::vector<BrowsePath> inputs) = 0;
};

inline Awaitable<BrowseResult> Browse(ViewService& view_service,
                                      scada::ServiceContext context,
                                      BrowseDescription input) {
  auto results =
      co_await view_service.Browse(std::move(context), {std::move(input)});
  if (!results.ok()) {
    co_return BrowseResult{.status_code = results.status().code()};
  }
  assert(results->size() == 1);
  co_return std::move(results->front());
}

inline Awaitable<BrowsePathResult> TranslateBrowsePath(
    ViewService& view_service,
    BrowsePath browse_path) {
  auto results =
      co_await view_service.TranslateBrowsePaths({std::move(browse_path)});
  if (!results.ok()) {
    co_return BrowsePathResult{.status_code = results.status().code()};
  }
  assert(results->size() == 1);
  co_return std::move(results->front());
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
