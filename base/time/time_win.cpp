#ifdef _WIN32

#include "base/check.h"
#include "base/time/calendar.h"
#include "base/time/time.h"
#include "base/time/time_wire_codec.h"

#include "base/test/scoped_mock_clock_override.h"

#include <windows.h>

#include <chrono>
#include <cstring>
#include <limits>
#include <optional>

namespace scada::base {

namespace {

// FILETIME is 100-ns ticks since the Windows 1601 epoch; these convert to/from
// microseconds since that same 1601 epoch.
int64_t FileTimeToMicroseconds1601(const FILETIME& ft) {
  int64_t result;
  static_assert(sizeof(result) == sizeof(ft));
  std::memcpy(&result, &ft, sizeof(result));
  return result / 10;  // 100-nanoseconds to microseconds.
}

void MicrosecondsToFileTime1601(int64_t us, FILETIME* ft) {
  scada::base::Check(us >= 0);
  int64_t val = us * 10;
  std::memcpy(ft, &val, sizeof(*ft));
}

// Microseconds since the Windows 1601 epoch for an absolute time.
int64_t ToMicroseconds1601(Time time) {
  return time.time_since_epoch().count() + kTimeTToMicrosecondsOffset;
}

Time FromMicroseconds1601(int64_t us_1601) {
  return Time{std::chrono::microseconds{us_1601 - kTimeTToMicrosecondsOffset}};
}

bool SafeConvertToWord(int in, WORD* out) {
  if (in < 0 || in > std::numeric_limits<WORD>::max()) {
    *out = std::numeric_limits<WORD>::max();
    return false;
  }
  *out = static_cast<WORD>(in);
  return true;
}

}  // namespace

Time NowUtc() {
  if (auto* mock = ScopedMockClockOverride::current())
    return mock->Now();
  FILETIME ft;
  ::GetSystemTimePreciseAsFileTime(&ft);
  return FromMicroseconds1601(FileTimeToMicroseconds1601(ft));
}

Exploded LocalExplode(Time time) {
  Exploded exploded = {};
  const int64_t us_1601 = ToMicroseconds1601(time);
  if (us_1601 < 0LL)
    return exploded;

  FILETIME utc_ft;
  MicrosecondsToFileTime1601(us_1601, &utc_ft);

  SYSTEMTIME st = {0};
  SYSTEMTIME utc_st;
  const bool success = FileTimeToSystemTime(&utc_ft, &utc_st) &&
                       SystemTimeToTzSpecificLocalTime(nullptr, &utc_st, &st);
  if (!success) {
    base::Check(false, "Unable to convert time");
    return exploded;
  }

  exploded.year = st.wYear;
  exploded.month = st.wMonth;
  exploded.day_of_week = st.wDayOfWeek;
  exploded.day_of_month = st.wDay;
  exploded.hour = st.wHour;
  exploded.minute = st.wMinute;
  exploded.second = st.wSecond;
  exploded.millisecond = st.wMilliseconds;
  return exploded;
}

std::optional<Time> FromLocalExploded(const Exploded& exploded) {
  SYSTEMTIME st;
  if (!SafeConvertToWord(exploded.year, &st.wYear) ||
      !SafeConvertToWord(exploded.month, &st.wMonth) ||
      !SafeConvertToWord(exploded.day_of_week, &st.wDayOfWeek) ||
      !SafeConvertToWord(exploded.day_of_month, &st.wDay) ||
      !SafeConvertToWord(exploded.hour, &st.wHour) ||
      !SafeConvertToWord(exploded.minute, &st.wMinute) ||
      !SafeConvertToWord(exploded.second, &st.wSecond) ||
      !SafeConvertToWord(exploded.millisecond, &st.wMilliseconds)) {
    return std::nullopt;
  }

  FILETIME ft;
  SYSTEMTIME utc_st;
  const bool success = TzSpecificLocalTimeToSystemTime(nullptr, &st, &utc_st) &&
                       SystemTimeToFileTime(&utc_st, &ft);
  if (!success)
    return std::nullopt;

  return FromMicroseconds1601(FileTimeToMicroseconds1601(ft));
}

// FILETIME wire codec (declared in base/time/time_wire_codec.h).
FILETIME EncodeFileTime(Time time) {
  FILETIME ft;
  MicrosecondsToFileTime1601(ToMicroseconds1601(time), &ft);
  return ft;
}

Time DecodeFileTime(FILETIME ft) {
  return FromMicroseconds1601(FileTimeToMicroseconds1601(ft));
}

}  // namespace scada::base

#endif  // _WIN32
