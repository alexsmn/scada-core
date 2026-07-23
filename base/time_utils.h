#pragma once

#include "base/time/time.h"
#include <chrono>

std::string SerializeToString(scada::base::TimeDelta delta);
bool Deserialize(std::string_view str, scada::base::TimeDelta& delta);

std::string SerializeToString(scada::base::Time time);
bool Deserialize(std::string_view str, scada::base::Time& time);

template <class Rep, class Period>
inline auto InMilliseconds(const std::chrono::duration<Rep, Period>& duration) {
  return std::chrono::duration_cast<std::chrono::milliseconds>(duration)
      .count();
}

template <class Rep, class Period>
inline auto InSeconds(const std::chrono::duration<Rep, Period>& duration) {
  return std::chrono::duration_cast<std::chrono::seconds>(duration).count();
}

template <class Rep, class Period>
inline auto InMicroseconds(const std::chrono::duration<Rep, Period>& duration) {
  return std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
}

// Truncates `time` down to a multiple of `interval` measured from the Unix
// epoch. Example: TruncateTimeTo(base::NowUtc(), std::chrono::seconds{1}).
inline scada::base::Time TruncateTimeTo(scada::base::Time time,
                                        scada::base::TimeDelta interval) {
  return time - (time.time_since_epoch() % interval);
}
