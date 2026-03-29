#include "base/time/time.h"

#include <cassert>
#include <cmath>
#include <format>
#include <limits>
#include <ostream>

namespace base {

// TimeDelta ------------------------------------------------------------------

int TimeDelta::InDays() const {
  if (is_max())
    return std::numeric_limits<int>::max();
  return static_cast<int>(delta_ / Time::kMicrosecondsPerDay);
}

int TimeDelta::InDaysFloored() const {
  if (is_max())
    return std::numeric_limits<int>::max();
  int result = static_cast<int>(delta_ / Time::kMicrosecondsPerDay);
  int64_t remainder = delta_ - (result * Time::kMicrosecondsPerDay);
  if (remainder < 0)
    --result;
  return result;
}

int TimeDelta::InHours() const {
  if (is_max())
    return std::numeric_limits<int>::max();
  return static_cast<int>(delta_ / Time::kMicrosecondsPerHour);
}

int TimeDelta::InMinutes() const {
  if (is_max())
    return std::numeric_limits<int>::max();
  return static_cast<int>(delta_ / Time::kMicrosecondsPerMinute);
}

double TimeDelta::InSecondsF() const {
  if (is_max())
    return std::numeric_limits<double>::infinity();
  return static_cast<double>(delta_) / Time::kMicrosecondsPerSecond;
}

int64_t TimeDelta::InSeconds() const {
  if (is_max())
    return std::numeric_limits<int64_t>::max();
  return delta_ / Time::kMicrosecondsPerSecond;
}

double TimeDelta::InMillisecondsF() const {
  if (is_max())
    return std::numeric_limits<double>::infinity();
  return static_cast<double>(delta_) / Time::kMicrosecondsPerMillisecond;
}

int64_t TimeDelta::InMilliseconds() const {
  if (is_max())
    return std::numeric_limits<int64_t>::max();
  return delta_ / Time::kMicrosecondsPerMillisecond;
}

int64_t TimeDelta::InMillisecondsRoundedUp() const {
  if (is_max())
    return std::numeric_limits<int64_t>::max();
  int64_t result = delta_ / Time::kMicrosecondsPerMillisecond;
  int64_t remainder = delta_ - (result * Time::kMicrosecondsPerMillisecond);
  if (remainder > 0)
    ++result;
  return result;
}

int64_t TimeDelta::InMicroseconds() const {
  if (is_max())
    return std::numeric_limits<int64_t>::max();
  return delta_;
}

double TimeDelta::InMicrosecondsF() const {
  if (is_max())
    return std::numeric_limits<double>::infinity();
  return static_cast<double>(delta_);
}

int64_t TimeDelta::InNanoseconds() const {
  if (is_max())
    return std::numeric_limits<int64_t>::max();
  return delta_ * Time::kNanosecondsPerMicrosecond;
}

namespace time_internal {

int64_t SaturatedAdd(TimeDelta delta, int64_t value) {
  int64_t a = delta.delta_;
  // Check for overflow: both positive and sum wraps negative.
  if (a > 0 && value > 0 && a > std::numeric_limits<int64_t>::max() - value)
    return std::numeric_limits<int64_t>::max();
  // Check for underflow: both negative and sum wraps positive.
  if (a < 0 && value < 0 && a < std::numeric_limits<int64_t>::min() - value)
    return std::numeric_limits<int64_t>::min();
  return a + value;
}

int64_t SaturatedSub(TimeDelta delta, int64_t value) {
  int64_t a = delta.delta_;
  // Check for overflow: a positive, value negative, result wraps.
  if (a > 0 && value < 0 && a > std::numeric_limits<int64_t>::max() + value)
    return std::numeric_limits<int64_t>::max();
  // Check for underflow: a negative, value positive, result wraps.
  if (a < 0 && value > 0 && a < std::numeric_limits<int64_t>::min() + value)
    return std::numeric_limits<int64_t>::min();
  return a - value;
}

}  // namespace time_internal

std::ostream& operator<<(std::ostream& os, TimeDelta time_delta) {
  return os << time_delta.InSecondsF() << " s";
}

// Time -----------------------------------------------------------------------

Time Time::FromDeltaSinceWindowsEpoch(TimeDelta delta) {
  return Time(delta.InMicroseconds());
}

TimeDelta Time::ToDeltaSinceWindowsEpoch() const {
  return TimeDelta::FromMicroseconds(us_);
}

Time Time::FromDoubleT(double dt) {
  if (dt == 0 || std::isnan(dt))
    return Time();
  return Time(static_cast<int64_t>(dt * kMicrosecondsPerSecond) +
              kTimeTToMicrosecondsOffset);
}

double Time::ToDoubleT() const {
  if (is_null())
    return 0;
  return static_cast<double>(us_ - kTimeTToMicrosecondsOffset) /
         kMicrosecondsPerSecond;
}

Time Time::UnixEpoch() {
  Time time;
  time.us_ = kTimeTToMicrosecondsOffset;
  return time;
}

Time Time::LocalMidnight() const {
  Exploded exploded;
  LocalExplode(&exploded);
  exploded.hour = 0;
  exploded.minute = 0;
  exploded.second = 0;
  exploded.millisecond = 0;
  Time out_time;
  if (FromLocalExploded(exploded, &out_time))
    return out_time;
  assert(false && "LocalMidnight failed");
  return Time();
}

bool Time::FromStringInternal(const char* time_string,
                              bool is_local,
                              Time* parsed_time) {
  // TODO: Implement without NSPR (PR_ParseTimeString).
  assert(false && "Time::FromString not implemented");
  return false;
}

bool Time::ExplodedMostlyEquals(const Exploded& lhs, const Exploded& rhs) {
  return lhs.year == rhs.year && lhs.month == rhs.month &&
         lhs.day_of_month == rhs.day_of_month && lhs.hour == rhs.hour &&
         lhs.minute == rhs.minute && lhs.second == rhs.second &&
         lhs.millisecond == rhs.millisecond;
}

std::ostream& operator<<(std::ostream& os, Time time) {
  Time::Exploded exploded;
  time.UTCExplode(&exploded);
  return os << std::format("{:04}-{:02}-{:02} {:02}:{:02}:{:02}.{:03} UTC",
                           exploded.year, exploded.month, exploded.day_of_month,
                           exploded.hour, exploded.minute, exploded.second,
                           exploded.millisecond);
}

inline bool is_in_range(int value, int lo, int hi) {
  return lo <= value && value <= hi;
}

bool Time::Exploded::HasValidValues() const {
  return is_in_range(month, 1, 12) && is_in_range(day_of_week, 0, 6) &&
         is_in_range(day_of_month, 1, 31) && is_in_range(hour, 0, 23) &&
         is_in_range(minute, 0, 59) && is_in_range(second, 0, 60) &&
         is_in_range(millisecond, 0, 999);
}

// TimeTicks ------------------------------------------------------------------

std::ostream& operator<<(std::ostream& os, TimeTicks time_ticks) {
  const TimeDelta as_time_delta = time_ticks - TimeTicks();
  return os << as_time_delta.InMicroseconds() << " bogo-microseconds";
}

}  // namespace base
