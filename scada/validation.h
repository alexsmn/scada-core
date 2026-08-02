#pragma once

#include "scada/view_service.h"

#include <algorithm>
#include <span>
#include <vector>

// Structural validators, available in all build types. They only report
// validity - callers decide whether a failure is an internal invariant
// (enforce with base::Check) or malformed external data (log and degrade).

// Returns true if |container| holds no duplicate elements under |comp|.
template <class Container, class Comp = std::ranges::less>
inline bool ValidateUnique(const Container& container, Comp comp = {}) {
  std::vector<typename Container::value_type> sorted_container(container);
  std::ranges::sort(sorted_container, std::move(comp));

  return std::ranges::adjacent_find(sorted_container, std::equal_to{}) ==
         sorted_container.end();
}

// Returns true if a browse result is structurally valid: a failed result
// carries no references, and a good result has no duplicate references.
inline bool Validate(const scada::BrowseResult& result) {
  if (scada::IsGood(result.status_code)) {
    return ValidateUnique(
        result.references, [](const scada::ReferenceDescription& a,
                              const scada::ReferenceDescription& b) {
          return std::tie(a.reference_type_id, a.forward, a.node_id) <
                 std::tie(b.reference_type_id, b.forward, b.node_id);
        });
  } else {
    return result.references.empty();
  }
}

// Returns true if every browse result in |results| is valid.
inline bool Validate(std::span<const scada::BrowseResult> results) {
  return std::ranges::all_of(results, [](const scada::BrowseResult& result) {
    return Validate(result);
  });
}
