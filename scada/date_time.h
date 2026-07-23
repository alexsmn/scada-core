#pragma once

#include "base/time/time.h"

#include <ostream>
#include <string>

namespace scada {

// OPC UA domain vocabulary for the base time types. Higher-layer (non-base)
// code spells time in scada:: terms; the underlying definitions and the
// low-level wire/calendar helpers stay in scada::base (the foundation layer,
// which cannot depend on this header).
using DateTime = base::Time;      // std::chrono::sys_time<microseconds>
using Duration = base::TimeDelta;  // std::chrono::microseconds

// The "no timestamp" sentinel (0 ticks since the 1601 Windows epoch) and the
// unbounded-range sentinels, in scada:: spelling. Distinct from a
// default-constructed DateTime (the Unix epoch); test with IsNull().
inline constexpr DateTime kNullTime = base::kNullTime;
inline constexpr DateTime kMaxTime = base::kMaxTime;
inline constexpr DateTime kMinTime = base::kMinTime;

// The current wall-clock time in UTC (honors ScopedMockClockOverride in tests).
inline DateTime Now() {
  return base::NowUtc();
}

// True when `time` is the null sentinel.
constexpr bool IsNull(DateTime time) {
  return base::IsNull(time);
}

}  // namespace scada

std::string ToString(scada::DateTime time);
std::u16string ToString16(scada::DateTime time);
