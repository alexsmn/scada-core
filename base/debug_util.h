#pragma once

// WARNING: For `std::wstring` and `std::u16string` logging include
// `base/logging.h` directly. It's intentionally omitted to avoid heavy
// includes.

#include <map>
#include <optional>
#include <ostream>
#include <span>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

template <class A, class B>
std::ostream& operator<<(std::ostream& stream, const std::pair<A, B>& pair);

template <class T>
std::ostream& operator<<(std::ostream& stream, const std::vector<T>& v);

template <class K, class V>
std::ostream& operator<<(std::ostream& stream, const std::map<K, V>& map);

template <class K, class V>
std::ostream& operator<<(std::ostream& stream,
                         const std::unordered_map<K, V>& map);

template <class T>
std::ostream& operator<<(std::ostream& stream, std::span<T> span);

template <class T>
std::ostream& operator<<(std::ostream& stream, const std::optional<T>& v);

template <class T>
std::string ToString(const T& v);

template <class T>
std::wstring ToString16(const T& v);

std::string BitMaskToString(unsigned bit_mask,
                            std::span<const std::string_view> bit_strings);
