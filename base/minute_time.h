#pragma once

#include <base/time/time.h>
#include <windows.h>

namespace {
static const FILETIME MinsEpoch = {0xc89dc000,
                                   0x01c07385};  // 2001, 1 Jan, 00:00:00.000
}

inline unsigned TimeToMins(const base::Time& time) {
  return unsigned((time.ToInternalValue() - *(LONGLONG*)&MinsEpoch) / 60e7);
}

inline base::Time MinsToTime(unsigned mins) {
  int64_t time = mins * __int64(60e7) + *(__int64*)&MinsEpoch;
  return base::Time::FromInternalValue(time);
}
