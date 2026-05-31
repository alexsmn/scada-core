#pragma once

#include "base/time_utils.h"
#include "metrics/aggregated_metric.h"
#include "metrics/metric_value.h"

#include <chrono>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>

using MetricAttributes = std::unordered_map<std::string, std::string>;

// Holds sampled metric values and common key/value attributes describing them.
class Metrics {
 public:
  bool empty() const { return values_.empty() && attributes_.empty(); }

  // Sets a common attribute for all metric values in this collection.
  void SetAttribute(std::string_view attr_name, std::string value) {
    attributes_.insert_or_assign(std::string{attr_name}, std::move(value));
  }

  void Set(std::string_view metric_name, const MetricValue& value) {
    values_.insert_or_assign(std::string{metric_name}, ToMetricValue(value));
  }

  template <class T>
    requires(!std::is_same_v<std::decay_t<T>, MetricValue>)
  void Set(std::string_view metric_name, const T& value) {
    values_.insert_or_assign(std::string{metric_name}, ToMetricValue(value));
  }

  // Special support for duration converting it to milliseconds and adding
  // suffix.
  void Set(std::string_view metric_name, std::chrono::nanoseconds duration) {
    values_.insert_or_assign(std::string{metric_name} + "_ms",
                             InMilliseconds(duration));
  }

  // Metric names are in `snake_case`.
  template <class T>
  void Set(std::string_view metric_name, AggregatedMetric<T>& aggregated_metric) {
    if (aggregated_metric.empty()) {
      return;
    }

    const std::string base_name{metric_name};
    Set(base_name + ".count", static_cast<int64_t>(aggregated_metric.count()));
    Set(base_name + ".min", aggregated_metric.min());
    Set(base_name + ".max", aggregated_metric.max());
    Set(base_name + ".mean", aggregated_metric.mean());
    Set(base_name + ".sum", aggregated_metric.sum());

    aggregated_metric.reset();
  }

  // visitor: (const std::string& name, const MetricValue& value)
  template <class T>
  void Visit(T&& visitor) const {
    for (const auto& [name, value] : values_) {
      visitor(name, value);
    }
  }

  const std::unordered_map<std::string, MetricValue>& ToUnorderedMap() const {
    return values_;
  }

  // Returns common attributes attached to this metric collection.
  const MetricAttributes& attributes() const { return attributes_; }

 private:
  std::unordered_map<std::string, MetricValue> values_;
  MetricAttributes attributes_;
};
