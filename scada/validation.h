#pragma once

#if !defined(NDEBUG)

#include "scada/view_service.h"

#include <algorithm>
#include <span>

template <class Container, class Comp = std::ranges::less>
inline bool ValidateUnique(const Container& container, Comp comp = {}) {
  std::vector<typename Container::value_type> sorted_container(container);
  std::ranges::sort(sorted_container, std::move(comp));

  bool contains_duplicates =
      std::ranges::adjacent_find(sorted_container, std::equal_to{}) !=
      sorted_container.end();

  assert(!contains_duplicates);
  return !contains_duplicates;
}

inline bool Validate(const scada::BrowseResult& result) {
  if (scada::IsGood(result.status_code)) {
    return ValidateUnique(
        result.references, [](const scada::ReferenceDescription& a,
                              const scada::ReferenceDescription& b) {
          return std::tie(a.reference_type_id, a.forward, a.node_id) <
                 std::tie(b.reference_type_id, b.forward, b.node_id);
        });
  } else {
    assert(result.references.empty());
    return result.references.empty();
  }
}

inline bool Validate(std::span<const scada::BrowseResult> results) {
  return std::ranges::all_of(results, [](const scada::BrowseResult& result) {
    return Validate(result);
  });
}

#endif
