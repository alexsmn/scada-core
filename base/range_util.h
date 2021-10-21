#pragma once

#include <algorithm>
#include <map>
#include <numeric>
#include <set>
#include <vector>

template <typename T>
using element_type_t =
    std::remove_reference_t<decltype(*std::begin(std::declval<T&>()))>;

template <class R>
inline auto Join(const R& sub_ranges) {
  std::vector<std::remove_const_t<element_type_t<element_type_t<R>>>> result;
  for (const auto& sub_range : sub_ranges)
    result.insert(result.end(), std::begin(sub_range), std::end(sub_range));
  return result;
}

template <class R>
inline auto Join(const R& range1, const R& range2) {
  std::vector<std::remove_const_t<element_type_t<R>>> result;
  result.reserve(range1.size() + range2.size());
  result.insert(result.end(), range1.begin(), range1.end());
  result.insert(result.end(), range2.begin(), range2.end());
  return result;
}

template <class R>
inline auto Join(const R& range1, const R& range2, const R& range3) {
  std::vector<std::remove_const_t<element_type_t<R>>> result;
  result.reserve(range1.size() + range2.size() + range3.size());
  result.insert(result.end(), range1.begin(), range1.end());
  result.insert(result.end(), range2.begin(), range2.end());
  result.insert(result.end(), range3.begin(), range3.end());
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

template <class R, class E>
inline bool Contains(const R& range, const E& item) {
  return std::find(std::cbegin(range), std::cend(range), item) !=
         std::cend(range);
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

template <class Range, class Mapper>
inline auto Group(Range&& range, const Mapper& mapper) {
  using Element = std::remove_const_t<element_type_t<Range>>;
  using Key = std::invoke_result_t<Mapper, Element>;
  std::map<Key, std::vector<Element>> result;
  for (auto&& e : range) {
    auto key = mapper(e);
    result[key].emplace_back(std::move(e));
  }
  return result;
}

// to_set

namespace detail {
struct to_set_forwarder {};
};  // namespace detail

template <class R>
inline auto operator|(const R& r, ::detail::to_set_forwarder) {
  return std::set<typename R::value_type>(std::begin(r), std::end(r));
}

inline static const auto to_set = ::detail::to_set_forwarder();

// to_vector

namespace detail {
struct to_vector_forwarder {};
};  // namespace detail

template <class R>
inline auto operator|(const R& r, ::detail::to_vector_forwarder) {
  return std::vector<typename R::value_type>(std::begin(r), std::end(r));
}

inline static const auto to_vector = ::detail::to_vector_forwarder();

// flattened

namespace detail {
struct flattened_forwarder {};
};  // namespace detail

template <class R>
inline auto operator|(const R& r, ::detail::flattened_forwarder) {
  return Join(r);
}

inline static const auto flattened = ::detail::flattened_forwarder();

// grouped

namespace detail {

template <class Mapper>
struct grouped_forwarder {
  Mapper mapper;
};

};  // namespace detail

template <class R, class Mapper>
inline auto operator|(const R& r,
                      const ::detail::grouped_forwarder<Mapper>& forwarder) {
  return Group(r, forwarder.mapper);
}

template <class Mapper>
inline auto grouped(Mapper&& mapper) {
  return ::detail::grouped_forwarder<Mapper>{std::forward<Mapper>(mapper)};
}
