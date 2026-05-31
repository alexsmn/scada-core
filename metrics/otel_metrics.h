#pragma once

#include <chrono>
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <string_view>

namespace metrics {

using MetricAttributes = std::map<std::string, std::string>;

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

  static std::string ResolveEndpoint();

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

// Records measurements through an OpenTelemetry meter and caches instruments.
class Meter {
 public:
  explicit Meter(std::string meter_name);
  Meter(std::string meter_name, MetricAttributes attributes);
  ~Meter();

  Meter(const Meter&) = delete;
  Meter& operator=(const Meter&) = delete;

  void AddCounter(std::string_view metric_name,
                  std::uint64_t value,
                  const MetricAttributes& attributes = {});
  void AddUpDownCounter(std::string_view metric_name,
                        std::int64_t delta,
                        const MetricAttributes& attributes = {});
  void RecordHistogram(std::string_view metric_name,
                       double value,
                       const MetricAttributes& attributes = {});

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

}  // namespace metrics
