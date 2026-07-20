#pragma once

#include "base/time/time.h"

#include <cstdint>

namespace minute_time_internal {
// An anonymous namespace here would give MinsEpoch internal linkage in every
// including TU (an ODR trap for the inline functions below) and would make
// this header unusable inside a module's global module fragment.
constexpr int64_t MinsEpoch =
    0x01c07385c89dc000LL;  // 2001, 1 Jan, 00:00:00.000
}  // namespace minute_time_internal

inline unsigned TimeToMins(const scada::base::Time& time) {
  return static_cast<unsigned>(
      (time.ToInternalValue() - minute_time_internal::MinsEpoch) / 60e7);
}

inline scada::base::Time MinsToTime(unsigned mins) {
  int64_t time =
      static_cast<int64_t>(mins * 60e7) + minute_time_internal::MinsEpoch;
  return scada::base::Time::FromInternalValue(time);
}
