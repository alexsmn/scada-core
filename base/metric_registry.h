#pragma once

#include <functional>
#include <string_view>

namespace metrics {

struct MetricValue {};

class MetricSink {
 public:
  virtual ~MetricSink() = default;

  virtual void WriteMetric(std::string_view name, const MetricValue& value) = 0;
};

using MetricCallback = std::function<void(MetricSink& sink)>;

class MetricRegistry {
 public:
  using Metric = std::size_t;

  Metric RegisterMetric(const MetricCallback& callback);

 private:
  MetricSink& sink_;
};

class MetricTrigger {
 public:
  MetricTrigger(MetricRegistry& registry, const MetricCallback& callback);
};

}  // namespace metrics
