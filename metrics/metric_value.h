#pragma once

#include <variant>

using MetricValue = std::variant<std::int64_t, double>;

template <class T>
inline MetricValue ToMetricValue(const T& value) {
  return value;
}

template <>
inline MetricValue ToMetricValue(const size_t& value) {
  return static_cast<std::int64_t>(value);
}
