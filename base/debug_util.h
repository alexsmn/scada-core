#pragma once

// Umbrella header for the debug/streaming helpers that used to be defined here.
// Prefer including the specific header you need:
//   base/stream_utf.h      - wide / UTF-16 string stream adapters
//   base/container_dump.h  - base::AsList / AsDict / AsOpt for containers
//   base/bit_mask_string.h - base::BitMaskToString
//
// This header additionally provides ToString / ToString16. Raw std::
// containers, pairs, and optionals are no longer streamable through global
// operator<< shims — wrap them in base::AsList / AsDict / AsOpt (which also
// work with boost::log::formatting_ostream and std::format).

#include "base/bit_mask_string.h"  // IWYU pragma: export
#include "base/container_dump.h"   // IWYU pragma: export
#include "base/stream_utf.h"       // IWYU pragma: export
#include "base/utf_convert.h"  // UtfConvert, used by ToString16 below.

#include <sstream>
#include <string>

// --- ToString / ToString16 --------------------------------------------------

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
