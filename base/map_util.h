#pragma once

#include <algorithm>
#include <cstddef>
#include <ranges>
#include <type_traits>
#include <vector>

namespace internal {

template <typename Map>
using KeyType = typename Map::key_type;

template <typename Map>
using MappedType = typename Map::mapped_type;

template <typename Map>
concept NullableMappedType =
    std::is_copy_constructible_v<MappedType<Map>> &&
    std::is_constructible_v<MappedType<Map>, std::nullptr_t>;

}  // namespace internal

template <typename Map, typename Key>
  requires internal::NullableMappedType<Map>
// Returns the mapped pointer-like value for `key`, or null when absent.
constexpr internal::MappedType<Map> FindOrNull(const Map& map, const Key& key) {
  auto it = map.find(key);
  return it != map.end() ? it->second : nullptr;
}

template <typename Map, typename Key>
  requires internal::NullableMappedType<Map>
// Returns the mapped pointer-like value for `key`, or null when absent.
constexpr internal::MappedType<Map> FindOrNull(Map& map, const Key& key) {
  auto it = map.find(key);
  return it != map.end() ? it->second : nullptr;
}

template <typename Map, typename Key>
// Returns a pointer to the mapped value for `key`, or null when absent.
constexpr const internal::MappedType<Map>* FindPtr(const Map& map,
                                                   const Key& key) {
  auto it = map.find(key);
  return it != map.end() ? &it->second : nullptr;
}

template <typename Map, typename Key>
// Returns a pointer to the mapped value for `key`, or null when absent.
constexpr internal::MappedType<Map>* FindPtr(Map& map, const Key& key) {
  auto it = map.find(key);
  return it != map.end() ? &it->second : nullptr;
}

template <class Map>
inline std::vector<internal::KeyType<Map>> GetKeyVector(const Map& map) {
  std::vector<internal::KeyType<Map>> result;
  result.reserve(map.size());
  std::ranges::copy(map | std::views::keys, std::back_inserter(result));
  return result;
}
