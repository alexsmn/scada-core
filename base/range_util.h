#pragma once

#include <algorithm>
#include <map>
#include <set>
#include <vector>

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

namespace detail {
struct to_set_forwarder {};
struct to_vector_forwarder {};
};  // namespace detail

template <class R>
inline auto operator|(const R& r, ::detail::to_set_forwarder) {
  return std::set<R::value_type>(std::begin(r), std::end(r));
}

template <class R>
inline auto operator|(const R& r, ::detail::to_vector_forwarder) {
  return std::vector<R::value_type>(std::begin(r), std::end(r));
}

inline static const auto to_set = ::detail::to_set_forwarder();
inline static const auto to_vector = ::detail::to_vector_forwarder();
