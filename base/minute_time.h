#pragma once

#include "base/time/time.h"

#include <cstdint>

namespace {
constexpr int64_t MinsEpoch =
    0x01c07385c89dc000LL;  // 2001, 1 Jan, 00:00:00.000
}

inline unsigned TimeToMins(const base::Time& time) {
  return static_cast<unsigned>((time.ToInternalValue() - MinsEpoch) / 60e7);
}

inline base::Time MinsToTime(unsigned mins) {
  int64_t time = static_cast<int64_t>(mins * 60e7) + MinsEpoch;
  return base::Time::FromInternalValue(time);
}
