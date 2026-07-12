#pragma once

// Version-stable codecs for the on-wire / on-disk time encodings.
//
// The int64 "microseconds since the Windows 1601-01-01 epoch" value that Time
// and TimeDelta expose via ToInternalValue()/FromInternalValue() is a de-facto
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

#include <cstdint>

#include "base/time/time.h"

namespace scada::base {

// --- microseconds since the Windows 1601-01-01 epoch (int64) ----------------
// Used by: gRPC (core/remote), SQLite history value/event rows, and the history
// sync resume cursor (stringified).

// Encodes an absolute time as the int64 µs-since-1601 wire value.
inline int64_t EncodeWireMicroseconds(Time time) {
  return time.ToInternalValue();
}
// Decodes an int64 µs-since-1601 wire value back into an absolute time.
inline Time DecodeWireTime(int64_t wire_us) {
  return Time::FromInternalValue(wire_us);
}

// Signed durations (e.g. history read intervals) share the same µs scale.
inline int64_t EncodeWireMicroseconds(TimeDelta delta) {
  return delta.ToInternalValue();
}
// Decodes an int64 µs wire value back into a signed duration.
inline TimeDelta DecodeWireDelta(int64_t wire_us) {
  return TimeDelta::FromInternalValue(wire_us);
}

// --- double seconds since the Unix 1970-01-01 epoch -------------------------
// Used by: saved graph-view files and CSV export. This is a DIFFERENT epoch
// (Unix, not 1601) and a lossy (double) representation than the µs-1601 wire
// value above — keep the two codecs distinct. Note the historical quirk that a
// null Time encodes to 0.0 and 0.0 decodes back to a null Time (not the Unix
// epoch); this is preserved deliberately.

// Encodes an absolute time as double seconds since the Unix epoch.
inline double EncodeDoubleT(Time time) {
  return time.ToDoubleT();
}
// Decodes double seconds since the Unix epoch back into an absolute time.
inline Time DecodeDoubleT(double unix_seconds) {
  return Time::FromDoubleT(unix_seconds);
}

#ifdef _WIN32
// --- FILETIME: 100-nanosecond ticks since the Windows 1601 epoch ------------
// Used by: classic OPC, Vidicon, and filesystem sync (Windows only).

// Encodes an absolute time as a Win32 FILETIME (100-ns ticks since 1601).
inline FILETIME EncodeFileTime(Time time) {
  return time.ToFileTime();
}
// Decodes a Win32 FILETIME (100-ns ticks since 1601) into an absolute time.
inline Time DecodeFileTime(FILETIME ft) {
  return Time::FromFileTime(ft);
}
#endif

}  // namespace scada::base
