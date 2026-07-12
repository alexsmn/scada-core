#pragma once

#include <boost/signals2/connection.hpp>

#include <chrono>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

#include "metrics/meter.h"
#include "metrics/metric_value.h"

#include "metrics/metrics_compat.h"
namespace scada::metrics {

struct MetricPoint {
  std::string scope_name;
  std::string metric_name;
  MetricAttributes attributes;
  MetricValue value;
};

using MetricValueObserver = std::function<void(const MetricPoint& point)>;

// Defines OpenTelemetry metric exporter and resource settings for the process.
struct OpenTelemetryMetricsOptions {
  std::string service_name;
  std::chrono::milliseconds export_interval{std::chrono::seconds{1}};
  std::chrono::milliseconds export_timeout{std::chrono::milliseconds{500}};
  std::string endpoint;
};

// Owns the process OpenTelemetry meter provider and OTLP metric exporter.
class OpenTelemetryMetrics {
 public:
  explicit OpenTelemetryMetrics(OpenTelemetryMetricsOptions options);
  ~OpenTelemetryMetrics();

  OpenTelemetryMetrics(const OpenTelemetryMetrics&) = delete;
  OpenTelemetryMetrics& operator=(const OpenTelemetryMetrics&) = delete;

  std::optional<MetricValue> GetMetricValue(std::string_view scope_name,
                                            std::string_view metric_name,
                                            std::string_view attribute_name,
                                            std::string_view attribute_value);

  boost::signals2::connection AddMetricValueObserver(
      MetricValueObserver observer);

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

}  // namespace scada::metrics
