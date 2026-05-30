#pragma once

#include <type_traits>
#include <variant>

using MetricValue = std::variant<std::int64_t, double>;

template <class T>
  requires(!std::is_integral_v<std::decay_t<T>> ||
           std::is_same_v<std::decay_t<T>, bool>)
inline MetricValue ToMetricValue(const T& value) {
  return value;
}

template <class T>
  requires(std::is_integral_v<std::decay_t<T>> &&
           !std::is_same_v<std::decay_t<T>, bool>)
inline MetricValue ToMetricValue(const T& value) {
  return static_cast<std::int64_t>(value);
}
