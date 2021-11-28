#pragma once

#include "base/containers/span.h"

#if !defined(NDEBUG)
inline bool Validate(base::span<const scada::ReferenceDescription> references) {
  std::vector<scada::ReferenceDescription> sorted_references(references.begin(),
                                                             references.end());
  std::sort(sorted_references.begin(), sorted_references.end(),
            [](const scada::ReferenceDescription& a,
               const scada::ReferenceDescription& b) {
              return std::tie(a.reference_type_id, a.forward, a.node_id) <
                     std::tie(b.reference_type_id, b.forward, b.node_id);
            });
  bool contain_duplicates =
      std::adjacent_find(sorted_references.begin(), sorted_references.end(),
                         std::equal_to{}) != sorted_references.end();
  assert(!contain_duplicates);
  return !contain_duplicates;
}

inline bool Validate(const scada::BrowseResult& result) {
  if (scada::IsGood(result.status_code)) {
    return Validate(result.references);
  } else {
    assert(result.references.empty());
    return result.references.empty();
  }
}

inline bool Validate(base::span<const scada::BrowseResult> results) {
  return std::all_of(
      results.begin(), results.end(),
      [](const scada::BrowseResult& result) { return Validate(result); });
}
#endif
