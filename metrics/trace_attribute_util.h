#pragma once

#include <cstddef>
#include <format>
#include <string>

#include "metrics/metrics_compat.h"
namespace scada::metrics {

// Joins the string forms of a batched request's node ids into one span
// attribute value, capped so huge batches don't bloat span storage: the
// first `max_items` entries are comma-joined and the remainder collapses to
// "+N". Callers pass any range plus a to-string projection, e.g.
//   JoinForAttribute(inputs, [](const auto& v) { return v.node_id.ToString();
//   })
template <class Range, class ToString>
std::string JoinForAttribute(const Range& range,
                             ToString&& to_string,
                             size_t max_items = 5) {
  std::string result;
  size_t index = 0;
  for (const auto& item : range) {
    if (index == max_items) {
      result += std::format(",+{}", std::size(range) - max_items);
      break;
    }
    if (index > 0) {
      result += ',';
    }
    result += to_string(item);
    ++index;
  }
  return result;
}

}  // namespace scada::metrics
