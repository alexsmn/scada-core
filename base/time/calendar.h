#pragma once

// Calendar / string conversions for absolute times, as free functions.
//
// These are deliberately non-members so they can survive a future change of the
// concrete time representation (the time class is being migrated to a
// std::chrono type, which cannot carry member functions).
//
// Target implementation split, once the type is std::chrono-backed:
//   - UTC conversions (UtcExplode/FromUtcExploded) become pure std::chrono
//     (year_month_day + hh_mm_ss over sys_days) — portable, no platform code.
//   - Local conversions (LocalExplode/FromLocalExploded) stay on the OS
//     (localtime_r / SystemTimeToTzSpecificLocalTime) because libc++ ships no
//     tzdb, so std::chrono::zoned_time / current_zone are unavailable.
// Today these are thin delegators over the existing OS-based Time members; the
// seam is what lets the swap happen later without touching callers.

#include "base/time/exploded.h"
#include "base/time/time.h"

#include <optional>
#include <string_view>

namespace scada::base {

// Breaks `time` down into UTC calendar fields.
Exploded UtcExplode(Time time);

// Breaks `time` down into local-time calendar fields.
Exploded LocalExplode(Time time);

// Rebuilds an absolute time from UTC calendar fields, or nullopt if the fields
// do not round-trip (e.g. out-of-range values).
std::optional<Time> FromUtcExploded(const Exploded& exploded);

// Rebuilds an absolute time from local-time calendar fields, or nullopt if the
// fields do not round-trip.
std::optional<Time> FromLocalExploded(const Exploded& exploded);

// Returns local midnight (00:00:00.000 local time) of the day containing
// `time`.
Time LocalMidnight(Time time);

// NowUtc() is declared in base/time/time.h.

// Parses a date/time string. When `is_local` is true a value without an
// explicit UTC/GMT suffix is interpreted in local time; otherwise it is always
// interpreted as UTC. Returns nullopt on parse failure.
std::optional<Time> TimeFromString(std::string_view str, bool is_local);

// Parses a date/time string, always interpreting it as UTC.
inline std::optional<Time> TimeFromUtcString(std::string_view str) {
  return TimeFromString(str, /*is_local=*/false);
}

// True when two exploded values agree on every field except day_of_week. Used
// by the From*Exploded implementations to reject component combinations that
// the OS / calendar would otherwise silently normalise (e.g. Feb 30).
bool ExplodedMostlyEquals(const Exploded& lhs, const Exploded& rhs);

}  // namespace scada::base
