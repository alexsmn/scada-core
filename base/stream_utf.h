#pragma once

// UTF stream adapters: stream wide (std::wstring / std::wstring_view) and
// UTF-16 (std::u16string / std::u16string_view) strings to a char-based stream
// by transcoding to UTF-8 first. These are global-scope operators so ordinary
// unqualified lookup finds them at the call site; they are deliberately not
// exported from the scada.base module (include this header where needed).
//
// The wide-string operators are templated on the stream type so a single
// definition serves both std::ostream and boost::log::formatting_ostream (a
// distinct type). The u16string operators are std::ostream-fixed; the
// boost::log::formatting_ostream counterparts live in base/boost_log.h next to
// the Boost.Log include they require, and are found via ADL for that stream.

#include "base/utf_convert.h"

#include <ostream>
#include <string>
#include <string_view>

// The return type is the stream expression's own type: for a derived stream
// such as std::ostringstream, `<< std::string_view` yields std::ostream&, so
// deducing `StreamT&` (via a comma expression) would fail to bind. Deduce from
// the insertion expression directly.
template <class StreamT>
inline auto operator<<(StreamT& stream, const std::wstring& s)
    -> decltype(stream << std::string_view{}) {
  return stream << UtfConvert<char>(s);
}

template <class StreamT>
inline auto operator<<(StreamT& stream, std::wstring_view s)
    -> decltype(stream << std::string_view{}) {
  return stream << UtfConvert<char>(s);
}

inline std::ostream& operator<<(std::ostream& os, std::u16string_view sv) {
  return os << UtfConvert<char>(sv);
}

inline std::ostream& operator<<(std::ostream& os, const std::u16string& str) {
  return os << UtfConvert<char>(std::u16string_view{str});
}
