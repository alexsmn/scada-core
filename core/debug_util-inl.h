#pragma once

#include "core/debug_util.h"

#include "base/strings/utf_string_conversions.h"

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
inline std::wstring ToString16(const T& v) {
  std::stringstream s;
  s << v;
  return base::UTF8ToUTF16(s.str());
}
