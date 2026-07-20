#pragma once

#include "base/time/time.h"
#include <chrono>

inline scada::base::TimeDelta TimeDeltaFromSecondsF(double dt) {
  return scada::base::TimeDelta::FromMicroseconds(static_cast<int64_t>(
      dt * static_cast<double>(scada::base::Time::kMicrosecondsPerSecond)));
}

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

template <class T>
inline auto AsChrono(scada::base::TimeDelta delta) {
  return std::chrono::duration_cast<T>(
      std::chrono::nanoseconds{delta.InNanoseconds()});
}

template <>
inline auto AsChrono<std::chrono::nanoseconds>(scada::base::TimeDelta delta) {
  return std::chrono::nanoseconds{delta.InNanoseconds()};
}

template <>
inline auto AsChrono<std::chrono::milliseconds>(scada::base::TimeDelta delta) {
  return std::chrono::milliseconds{delta.InMilliseconds()};
}

template <typename T = std::chrono::system_clock::time_point>
inline auto AsChrono(scada::base::Time time) {
  return T{
      AsChrono<typename T::duration>(time - scada::base::Time::UnixEpoch())};
}

// Example of truncation to a second:
//   TruncateTimeTo(base::Time::Now(), base::TimeDelta::FromSeconds(1));
inline scada::base::Time TruncateTimeTo(scada::base::Time time,
                                        scada::base::TimeDelta interval) {
  auto delta = time - scada::base::Time::UnixEpoch();
  auto remainder = scada::base::TimeDelta::FromNanoseconds(
      delta.InNanoseconds() % interval.InNanoseconds());
  return time - remainder;
}
