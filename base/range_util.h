#pragma once

#include <algorithm>
#include <set>
#include <vector>

template <class T, class Range>
inline std::vector<T> MakeVector(const Range& range) {
  return std::vector<T>(std::begin(range), std::end(range));
}

template <class C>
inline C Join(const std::vector<C>& sub_ranges) {
  C result;
  for (const auto& sub_range : sub_ranges)
    result.insert(result.end(), sub_range.begin(), sub_range.end());
  return result;
}

template <class T>
inline std::set<T> Union(const std::vector<std::set<T>>& subsets) {
  std::set<T> result;
  for (const auto& subset : subsets) {
    for (const auto& e : subset)
      result.insert(e);
  }
  return result;
}

template <class Range, class Func>
inline auto Map(const Range& range, const Func& func) {
  std::vector<decltype(func({}))> result;
  std::transform(std::cbegin(range), std::cend(range),
                 std::back_inserter(result), func);
  return result;
}
