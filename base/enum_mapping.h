#pragma once

#include <algorithm>
#include <optional>
#include <utility>

// Example:
//
// constexpr std::pair<cfg::Iec60870Protocol, Iec60870Protocol>
//     kProtocolMapping[] = {
//         {cfg::Iec60870Protocol::IEC101, Iec60870Protocol::IEC101},
//         {cfg::Iec60870Protocol::IEC104, Iec60870Protocol::IEC104},
// };
//
template <class A, class B, size_t N>
inline std::optional<B> MapEnum(const std::pair<A, B> (&mapping)[N], A a) {
  auto i = std::ranges::find(mapping, a, [](const auto& p) { return p.first; });
  if (i == std::end(mapping)) {
    return std::nullopt;
  }
  return i->second;
}
