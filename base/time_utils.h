#pragma once

#include <base/time/time.h>
#include <chrono>

inline base::TimeDelta TimeDeltaFromSecondsF(double dt) {
  return base::TimeDelta::FromMicroseconds(static_cast<int64_t>(
      dt * static_cast<double>(base::Time::kMicrosecondsPerSecond)));
}

std::string SerializeToString(base::TimeDelta delta);
bool Deserialize(std::string_view str, base::TimeDelta& delta);

std::string SerializeToString(base::Time time);
bool Deserialize(std::string_view str, base::Time& time);

inline auto InMilliseconds(std::chrono::nanoseconds duration) {
  return std::chrono::duration_cast<std::chrono::milliseconds>(duration)
      .count();
}

inline auto InSeconds(std::chrono::nanoseconds duration) {
  return std::chrono::duration_cast<std::chrono::seconds>(duration).count();
}

inline auto AsChrono(base::TimeDelta delta) {
  return std::chrono::nanoseconds{delta.InNanoseconds()};
}

// Example of truncation to a second:
//   TruncateTimeTo(base::Time::Now(), base::TimeDelta::FromSeconds(1));
inline base::Time TruncateTimeTo(base::Time time, base::TimeDelta interval) {
  auto delta = time - base::Time::UnixEpoch();
  auto remainder = base::TimeDelta::FromNanoseconds(delta.InNanoseconds() %
                                                    interval.InNanoseconds());
  return time - remainder;
}
