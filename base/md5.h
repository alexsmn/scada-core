#pragma once

// Minimal MD5 implementation replacing base::MD5String from ChromiumBase.
// Uses Boost.UUID's internal MD5 (header-only, no extra link dependency).

#include <boost/uuid/detail/md5.hpp>

#include <cstdint>
#include <cstdio>
#include <string>
#include <string_view>

namespace base {

inline std::string MD5String(std::string_view input) {
  boost::uuids::detail::md5 hasher;
  hasher.process_bytes(input.data(), input.size());

  boost::uuids::detail::md5::digest_type digest;
  hasher.get_digest(digest);

  // Each digest element is a 32-bit word; format as 32 hex chars.
  char hex[33];
  for (int i = 0; i < 4; ++i) {
    std::snprintf(hex + i * 8, 9, "%08x", digest[i]);
  }
  return std::string(hex, 32);
}

}  // namespace base
