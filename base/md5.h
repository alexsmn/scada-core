#pragma once

// Minimal MD5 implementation replacing base::MD5String from ChromiumBase.
// Uses Boost.UUID's internal MD5 (header-only, no extra link dependency).

#include <boost/uuid/detail/md5.hpp>

#include <cstdint>
#include <cstdio>
#include <string>
#include <string_view>

#include "base/base_compat.h"
namespace scada::base {

inline std::string MD5String(std::string_view input) {
  boost::uuids::detail::md5 hasher;
  hasher.process_bytes(input.data(), input.size());

  boost::uuids::detail::md5::digest_type digest;
  hasher.get_digest(digest);

  // digest_type changed across Boost releases: 4 x 32-bit words (already in
  // print order) in older Boost, 16 bytes in Boost >= 1.86. Handle both, or
  // the word-formatting path silently produces a wrong hash from a byte
  // array (it reads only the first 4 bytes and widens each to 8 hex chars).
  char hex[33];
  if constexpr (sizeof(digest[0]) == 1) {
    for (int i = 0; i < 16; ++i) {
      std::snprintf(hex + i * 2, 3, "%02x", static_cast<unsigned>(digest[i]));
    }
  } else {
    for (int i = 0; i < 4; ++i) {
      std::snprintf(hex + i * 8, 9, "%08x", static_cast<unsigned>(digest[i]));
    }
  }
  return std::string(hex, 32);
}

}  // namespace scada::base
