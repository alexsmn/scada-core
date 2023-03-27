#pragma once

#include "base/debug_util.h"

#include <boost/locale/encoding_utf.hpp>

template <class A, class B>
inline std::ostream& operator<<(std::ostream& stream,
                                const std::pair<A, B>& pair) {
  return stream << "{" << pair.first << ", " << pair.second << "}";
}

template <class T>
inline std::ostream& operator<<(std::ostream& stream, const std::vector<T>& v) {
  stream << "[";
  for (size_t i = 0; i < v.size(); ++i) {
    stream << v[i];
    if (i != v.size() - 1)
      stream << ", ";
  }
  stream << "]";
  return stream;
}

template <class K, class V>
inline std::ostream& operator<<(std::ostream& stream,
                                const std::map<K, V>& map) {
  stream << "{";
  auto i = map.begin();
  if (i != map.end()) {
    stream << i->first << ": " << i->second;
    for (; i != map.end(); ++i) {
      stream << ", ";
      stream << i->first << ": " << i->second;
    }
  }
  stream << "}";
  return stream;
}

template <class T>
inline std::ostream& operator<<(std::ostream& stream, std::span<T> span) {
  stream << "[";
  for (size_t i = 0; i < span.size(); ++i) {
    stream << span[i];
    if (i != span.size() - 1)
      stream << ", ";
  }
  stream << "]";
  return stream;
}

template <class T>
inline std::ostream& operator<<(std::ostream& stream, base::span<T> span) {
  return stream << std::span<T>{span.data(), span.size()};
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
  return boost::locale::conv::utf_to_utf<char16_t>(s.str());
}
