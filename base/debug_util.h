#pragma once

// Umbrella header for the debug/streaming helpers that used to be defined here.
// Prefer including the specific header you need:
//   base/stream_utf.h      - wide / UTF-16 string stream adapters
//   base/container_dump.h  - base::AsList / AsDict / AsOpt for containers
//   base/bit_mask_string.h - base::BitMaskToString
//
// This header additionally provides ToString / ToString16 and, transitionally,
// global operator<< overloads for std:: containers, std::pair, and
// std::optional. Those global overloads are found only by ordinary unqualified
// lookup (never ADL, since a std:: type's only associated namespace is std) and
// never served boost::log::formatting_ostream. New code should stream
// base::AsList / AsDict / AsOpt instead; these shims exist only so pre-split
// `stream << container` / `ToString(container)` call sites keep compiling while
// they migrate.

#include "base/bit_mask_string.h"
#include "base/container_dump.h"
#include "base/stream_utf.h"
#include "base/utf_convert.h"  // UtfConvert, used by ToString16 below.

#include <map>
#include <optional>
#include <ostream>
#include <span>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

// --- Transitional global operator<< shims (std::ostream only) ---------------
//
// These stream each element via operator<< (not std::format), so they keep
// working for element types that have an operator<< but no std::formatter —
// preserving the exact pre-split behavior. base::AsList / AsDict / AsOpt are the
// std::format-native replacement for new code.

// Leaf operators, declared before PrintList/PrintDict so those helpers see them
// via ordinary unqualified lookup at their template-definition point (e.g. when
// streaming a std::vector<std::pair<...>>); global operators are not reachable
// by ADL for std:: element types.
template <class A, class B>
inline std::ostream& operator<<(std::ostream& stream,
                                const std::pair<A, B>& pair) {
  return stream << "{" << pair.first << ", " << pair.second << "}";
}

template <class T>
inline std::ostream& operator<<(std::ostream& stream, const std::optional<T>& v) {
  if (v.has_value())
    stream << *v;
  else
    stream << "nullopt";
  return stream;
}

namespace internal {

template <class L>
inline void PrintList(const L& list, std::ostream& stream) {
  stream << "[";
  if (auto i = list.begin(); i != list.end()) {
    stream << *i;
    for (++i; i != list.end(); ++i) {
      stream << ", " << *i;
    }
  }
  stream << "]";
}

template <class D>
inline void PrintDict(const D& dict, std::ostream& stream) {
  stream << "{";
  if (auto i = dict.begin(); i != dict.end()) {
    stream << i->first << ": " << i->second;
    for (++i; i != dict.end(); ++i) {
      stream << ", " << i->first << ": " << i->second;
    }
  }
  stream << "}";
}

}  // namespace internal

template <class T>
inline std::ostream& operator<<(std::ostream& stream, const std::vector<T>& v) {
  ::internal::PrintList(v, stream);
  return stream;
}

template <class K, class V>
inline std::ostream& operator<<(std::ostream& stream,
                                const std::map<K, V>& map) {
  ::internal::PrintDict(map, stream);
  return stream;
}

template <class K, class V>
inline std::ostream& operator<<(std::ostream& stream,
                                const std::unordered_map<K, V>& map) {
  ::internal::PrintDict(map, stream);
  return stream;
}

template <class T>
inline std::ostream& operator<<(std::ostream& stream, std::span<T> span) {
  ::internal::PrintList(span, stream);
  return stream;
}

// --- ToString / ToString16 --------------------------------------------------
//
// Defined after the shims above so their template-definition-point lookup sees
// the container operator<< overloads (ADL cannot reach global operators for
// std:: types). Prefer wrapping containers in base::AsList / AsDict / AsOpt.

template <class T>
inline std::string ToString(const T& v) {
  std::stringstream s;
  s << v;
  return s.str();
}

template <class T>
inline std::u16string ToString16(const T& v) {
  std::stringstream s;
  s << v;
  return UtfConvert<char16_t>(s.str());
}
