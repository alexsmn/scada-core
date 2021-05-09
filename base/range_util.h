#pragma once

#include <algorithm>
#include <map>
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

template <class E, class T>
inline bool Erase(std::vector<E>& vector, const T& item) {
  auto i = std::find(std::begin(vector), std::end(vector), item);
  if (i == std::end(vector))
    return false;
  vector.erase(i);
  return true;
}

template <class K, class V, class T>
inline bool Erase(std::map<K, V>& map, const T& item) {
  return map.erase(item) != 0;
}

template <class K, class T>
inline bool Erase(std::set<K>& set, const T& item) {
  return set.erase(item) != 0;
}
