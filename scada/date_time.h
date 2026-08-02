#pragma once

#include "base/time/time.h"

#include <ostream>
#include <string>

namespace scada {

// OPC UA "now"/null vocabulary over the base time types. scada::Time and
// scada::Duration themselves are defined in base/time/time.h (the single
// spelling used everywhere); this header adds the domain helpers that the
// higher layers use. The low-level wire/calendar helpers stay in scada::base.

// The "no timestamp" sentinel (0 ticks since the 1601 Windows epoch) and the
// unbounded-range sentinels, in scada:: spelling. Distinct from a
// default-constructed Time (the Unix epoch); test with IsNull().
inline constexpr Time kNullTime = base::kNullTime;
inline constexpr Time kMaxTime = base::kMaxTime;
inline constexpr Time kMinTime = base::kMinTime;

// The current wall-clock time in UTC (honors ScopedMockClockOverride in tests).
inline Time Now() {
  return base::NowUtc();
}

// True when `time` is the null sentinel.
constexpr bool IsNull(Time time) {
  return base::IsNull(time);
}

}  // namespace scada

std::string ToString(scada::Time time);
std::u16string ToString16(scada::Time time);
