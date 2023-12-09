#pragma once

#include "metrics/aggregated_metric.h"
#include "metrics/metric_value.h"

#include <chrono>
#include <string>
#include <unordered_map>

class Metrics {
 public:
  bool empty() const { return values_.empty(); }

  void Set(const std::string& name, const MetricValue& value) {
    values_.insert_or_assign(name, value);
  }

  // Special support for duration.
  void Set(const std::string& name, std::chrono::nanoseconds duration) {
    values_.insert_or_assign(
        name, std::chrono::duration_cast<std::chrono::microseconds>(duration)
                  .count());
  }

  // Metric names are in `snake_case`.
  template <class T>
  void Collect(const std::string& name,
               AggregatedMetric<T>& aggregated_metric) {
    if (aggregated_metric.empty()) {
      return;
    }

    Set(name + ".count", static_cast<int64_t>(aggregated_metric.count()));
    Set(name + ".min", aggregated_metric.min());
    Set(name + ".max", aggregated_metric.max());
    Set(name + ".mean", aggregated_metric.mean());
    Set(name + ".total", aggregated_metric.total());

    aggregated_metric.reset();
  }

  // visitor: (const std::string& name, const MetricValue& value)
  template <class T>
  void Visit(T&& visitor) const {
    for (const auto& [name, value] : values_) {
      visitor(name, value);
    }
  }

  std::unordered_map<std::string, MetricValue> ToUnorderedMap() const {
    return values_;
  }

 private:
  std::unordered_map<std::string, MetricValue> values_;
};
