#ifndef _WIN32

#include "base/time/time.h"

#include "base/time/calendar.h"
#include "base/test/scoped_mock_clock_override.h"

#include <algorithm>
#include <chrono>
#include <cstring>
#include <ctime>
#include <limits>
#include <mutex>
#include <optional>
#include <sys/time.h>

namespace {

std::mutex& GetTimeStructLock() {
  static std::mutex lock;
  return lock;
}

// Local-time <-> struct tm. libc++ has no tzdb, so local conversions stay on
// the C library; UTC conversions are pure std::chrono in time.cpp.
time_t LocalTimeFromTimeStruct(struct tm* timestruct) {
  std::lock_guard guard(GetTimeStructLock());
  return mktime(timestruct);
}

void LocalTimeToTimeStruct(time_t t, struct tm* timestruct) {
  std::lock_guard guard(GetTimeStructLock());
  localtime_r(&t, timestruct);
}

}  // namespace

namespace scada::base {

Time NowUtc() {
  if (auto* mock = ScopedMockClockOverride::current())
    return mock->Now();
  struct timeval tv;
  struct timezone tz = {0, 0};
  gettimeofday(&tv, &tz);
  return Time{std::chrono::microseconds{tv.tv_sec * kMicrosecondsPerSecond +
                                        tv.tv_usec}};
}

Exploded LocalExplode(Time time) {
  const int64_t microseconds = time.time_since_epoch().count();

  int64_t milliseconds;
  time_t seconds;
  int millisecond;

  if (microseconds >= 0) {
    milliseconds = microseconds / kMicrosecondsPerMillisecond;
    seconds = static_cast<time_t>(milliseconds / kMillisecondsPerSecond);
    millisecond = static_cast<int>(milliseconds % kMillisecondsPerSecond);
  } else {
    milliseconds = (microseconds - kMicrosecondsPerMillisecond + 1) /
                   kMicrosecondsPerMillisecond;
    seconds = static_cast<time_t>(
        (milliseconds - kMillisecondsPerSecond + 1) / kMillisecondsPerSecond);
    millisecond = static_cast<int>(milliseconds % kMillisecondsPerSecond);
    if (millisecond < 0)
      millisecond += static_cast<int>(kMillisecondsPerSecond);
  }

  struct tm timestruct;
  LocalTimeToTimeStruct(seconds, &timestruct);

  Exploded exploded;
  exploded.year = timestruct.tm_year + 1900;
  exploded.month = timestruct.tm_mon + 1;
  exploded.day_of_week = timestruct.tm_wday;
  exploded.day_of_month = timestruct.tm_mday;
  exploded.hour = timestruct.tm_hour;
  exploded.minute = timestruct.tm_min;
  exploded.second = timestruct.tm_sec;
  exploded.millisecond = millisecond;
  return exploded;
}

std::optional<Time> FromLocalExploded(const Exploded& exploded) {
  struct tm timestruct;
  std::memset(&timestruct, 0, sizeof(timestruct));
  timestruct.tm_sec = exploded.second;
  timestruct.tm_min = exploded.minute;
  timestruct.tm_hour = exploded.hour;
  timestruct.tm_mday = exploded.day_of_month;
  timestruct.tm_mon = exploded.month - 1;
  timestruct.tm_year = exploded.year - 1900;
  timestruct.tm_wday = exploded.day_of_week;
  timestruct.tm_yday = 0;
  timestruct.tm_isdst = -1;
#if !defined(__sun)
  timestruct.tm_gmtoff = 0;
  timestruct.tm_zone = nullptr;
#endif

  struct tm timestruct0 = timestruct;
  time_t seconds = LocalTimeFromTimeStruct(&timestruct);

  if (seconds == -1) {
    timestruct = timestruct0;
    timestruct.tm_isdst = 0;
    int64_t seconds_isdst0 = LocalTimeFromTimeStruct(&timestruct);

    timestruct = timestruct0;
    timestruct.tm_isdst = 1;
    int64_t seconds_isdst1 = LocalTimeFromTimeStruct(&timestruct);

    if (seconds_isdst0 < 0)
      seconds = static_cast<time_t>(seconds_isdst1);
    else if (seconds_isdst1 < 0)
      seconds = static_cast<time_t>(seconds_isdst0);
    else
      seconds = static_cast<time_t>(std::min(seconds_isdst0, seconds_isdst1));
  }

  int64_t milliseconds = 0;
  if (seconds == -1 && (exploded.year < 1969 || exploded.year > 1970)) {
    if (exploded.year < 1969) {
      milliseconds =
          static_cast<int64_t>(std::numeric_limits<int32_t>::min()) *
          kMillisecondsPerSecond;
    } else {
      milliseconds =
          static_cast<int64_t>(std::numeric_limits<int32_t>::max()) *
              kMillisecondsPerSecond +
          (kMillisecondsPerSecond - 1);
    }
  } else {
    milliseconds = static_cast<int64_t>(seconds) * kMillisecondsPerSecond +
                   exploded.millisecond;
  }

  const Time result{
      std::chrono::microseconds{milliseconds * kMicrosecondsPerMillisecond}};

  if (ExplodedMostlyEquals(LocalExplode(result), exploded))
    return result;
  return std::nullopt;
}

}  // namespace scada::base

#endif  // !_WIN32
