#pragma once

// The project's absolute-time and signed-duration types, as std::chrono aliases:
//
//   scada::Duration = std::chrono::microseconds
//   scada::Time     = std::chrono::sys_time<microseconds>  (µs since the Unix epoch)
//
// This is the single spelling used everywhere, the base layer included; the OPC
// UA domain helpers (Now/IsNull, and the scada:: re-exports of the sentinels
// below) live in core/scada/date_time.h.
//
// Null timestamps: the historical "no value" instant is 0 ticks since the
// Windows 1601 epoch, which is kNullTime here — NOT the default-constructed
// Time, which under std::chrono is the Unix epoch (1970-01-01). Use IsNull()
// to test, and initialise "no timestamp" members with kNullTime.
//
// Calendar (UTC/local) conversions are the free functions in
// base/time/calendar.h; the persisted/transmitted encodings live behind
// base/time/time_wire_codec.h. Monotonic timing uses std::chrono::steady_clock
// directly (there is no TimeTicks type).

#include "base/time/exploded.h"

#include <chrono>
#include <cstdint>

#ifdef _WIN32
// Forward-declare FILETIME to avoid including <windows.h>.
typedef struct _FILETIME FILETIME;
#endif

namespace scada {

// A signed time interval. OPC UA Part 3 §8.13 Duration,
// https://reference.opcfoundation.org/Core/Part3/v105/docs/8.13
using Duration = std::chrono::microseconds;

// An absolute UTC instant at microsecond resolution. OPC UA Part 6 §5.2.2.5,
// https://reference.opcfoundation.org/Core/Part6/v105/docs/5.2.2.5
using Time = std::chrono::sys_time<Duration>;

}  // namespace scada

namespace scada::base {

// Time-unit conversion factors, retained for the low-level microsecond math in
// the time module and its wire/calendar helpers.
inline constexpr int64_t kMicrosecondsPerMillisecond = 1000;
inline constexpr int64_t kMillisecondsPerSecond = 1000;
inline constexpr int64_t kMicrosecondsPerSecond =
    kMicrosecondsPerMillisecond * kMillisecondsPerSecond;
inline constexpr int64_t kMicrosecondsPerMinute = kMicrosecondsPerSecond * 60;
inline constexpr int64_t kMicrosecondsPerHour = kMicrosecondsPerMinute * 60;
inline constexpr int64_t kMicrosecondsPerDay = kMicrosecondsPerHour * 24;
inline constexpr int64_t kMillisecondsPerDay =
    kMicrosecondsPerDay / kMicrosecondsPerMillisecond;
inline constexpr int64_t kNanosecondsPerMicrosecond = 1000;

// Microseconds from the Windows 1601-01-01 epoch to the Unix 1970-01-01 epoch.
inline constexpr int64_t kTimeTToMicrosecondsOffset =
    INT64_C(11644473600000000);

// The OPC UA null DateTime: 0 ticks since the 1601 Windows epoch. Distinct from
// the default-constructed Time (the Unix epoch), so a device reporting a real
// 1970-01-01 timestamp is never mistaken for "no value".
inline constexpr Time kNullTime{
    std::chrono::microseconds{-kTimeTToMicrosecondsOffset}};

// Unbounded-range sentinels: open history-read endpoints and the "current
// only" marker. They round-trip through the wire codec as INT64_MAX/MIN.
inline constexpr Time kMaxTime = Time::max();
inline constexpr Time kMinTime = Time::min();

// True when `time` is the null sentinel (the 1601 epoch).
constexpr bool IsNull(Time time) {
  return time == kNullTime;
}

// The current wall-clock time in UTC. Honors ScopedMockClockOverride in tests.
// Declared here (defined in the platform time source) so callers that only need
// "now" do not have to include base/time/calendar.h.
Time NowUtc();

}  // namespace scada::base
