#pragma once

#include "base/debug_util.h"

#include "base/strings/utf_ostream_operators.h"
#include "base/utf_convert.h"
#include <sstream>

namespace internal {

// TODO: Extend for forward-only ranges.
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
    for (; i != dict.end(); ++i) {
      stream << ", " << i->first << ": " << i->second;
    }
  }
  stream << "}";
}

}  // namespace internal

template <class A, class B>
inline std::ostream& operator<<(std::ostream& stream,
                                const std::pair<A, B>& pair) {
  return stream << "{" << pair.first << ", " << pair.second << "}";
}

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

template <class T>
inline std::ostream& operator<<(std::ostream& stream,
                                const std::optional<T>& v) {
  if (v.has_value())
    stream << *v;
  else
    stream << "nullopt";
  return stream;
}

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
