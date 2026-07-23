#pragma once

// Version-stable codecs for the on-wire / on-disk time encodings.
//
// The int64 "microseconds since the Windows 1601-01-01 epoch" value that Time
// and Duration expose via ToInternalValue()/FromInternalValue() is a de-facto
// wire and on-disk contract: it is serialized directly into gRPC messages,
// SQLite history rows, and the history sync resume cursor. Routing every such
// boundary through these named codecs — instead of calling ToInternalValue()
// inline — keeps the encoding pinned in one place, so a future change to Time's
// in-memory representation cannot silently alter persisted or transmitted
// timestamps.
//
// Each codec is currently an identity over the existing Time API; the value is
// the firewall (a single, named, testable boundary), not the arithmetic.
//
// Two other encodings already live behind their own single-purpose interfaces
// and are intentionally NOT duplicated here: the "minutes since 2001" index in
// base/minute_time.h, and the sentinel-aware OPC UA 100-ns tick conversion in
// common/opcua_bridge/conversion.h.

#include <chrono>
#include <cmath>
#include <cstdint>
#include <limits>

#include "base/time/time.h"

namespace scada::base {

// --- microseconds since the Windows 1601-01-01 epoch (int64) ----------------
// Used by: gRPC (core/remote), SQLite history value/event rows, and the history
// sync resume cursor (stringified).

// Encodes an absolute time as the int64 µs-since-1601 wire value. The
// unbounded-range sentinels map to INT64_MAX/MIN (their arithmetic value would
// overflow); the null sentinel (the 1601 epoch) maps to 0.
inline int64_t EncodeWireMicroseconds(Time time) {
  if (time == kMaxTime)
    return std::numeric_limits<int64_t>::max();
  if (time == kMinTime)
    return std::numeric_limits<int64_t>::min();
  return (time - kNullTime).count();
}
// Decodes an int64 µs-since-1601 wire value back into an absolute time.
inline Time DecodeWireTime(int64_t wire_us) {
  if (wire_us == std::numeric_limits<int64_t>::max())
    return kMaxTime;
  if (wire_us == std::numeric_limits<int64_t>::min())
    return kMinTime;
  return kNullTime + std::chrono::microseconds{wire_us};
}

// Signed durations (e.g. history read intervals) share the same µs scale.
inline int64_t EncodeWireMicroseconds(Duration delta) {
  return delta.count();
}
// Decodes an int64 µs wire value back into a signed duration.
inline Duration DecodeWireDelta(int64_t wire_us) {
  return Duration{wire_us};
}

// --- double seconds since the Unix 1970-01-01 epoch -------------------------
// Used by: saved graph-view files and CSV export. This is a DIFFERENT epoch
// (Unix, not 1601) and a lossy (double) representation than the µs-1601 wire
// value above — keep the two codecs distinct. Note the historical quirk that a
// null Time encodes to 0.0 and 0.0 decodes back to a null Time (not the Unix
// epoch); this is preserved deliberately.

// Encodes an absolute time as double seconds since the Unix epoch. A null Time
// encodes to 0.0 (the historical quirk preserved below).
inline double EncodeDoubleT(Time time) {
  if (IsNull(time))
    return 0.0;
  return std::chrono::duration<double>{time.time_since_epoch()}.count();
}
// Decodes double seconds since the Unix epoch back into an absolute time. 0.0
// (and NaN) decode back to a NULL time, not the Unix epoch — deliberately
// asymmetric with encoding to preserve the historical round-trip behavior.
inline Time DecodeDoubleT(double unix_seconds) {
  if (unix_seconds == 0.0 || std::isnan(unix_seconds))
    return kNullTime;
  return Time{} + std::chrono::duration_cast<std::chrono::microseconds>(
                      std::chrono::duration<double>{unix_seconds});
}

#ifdef _WIN32
// --- FILETIME: 100-nanosecond ticks since the Windows 1601 epoch ------------
// Used by: classic OPC, Vidicon, and filesystem sync (Windows only). Defined in
// base/time/time_win.cpp (they touch the FILETIME layout).

// Encodes an absolute time as a Win32 FILETIME (100-ns ticks since 1601).
FILETIME EncodeFileTime(Time time);
// Decodes a Win32 FILETIME (100-ns ticks since 1601) into an absolute time.
Time DecodeFileTime(FILETIME ft);
#endif

}  // namespace scada::base
