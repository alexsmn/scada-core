#include "metrics/otel_metrics.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <optional>

namespace metrics {
namespace {

class ScopedEnv {
 public:
  explicit ScopedEnv(const char* name) : name_{name} {
    const char* value = std::getenv(name_);
    if (value) {
      original_ = value;
    }
  }

  ~ScopedEnv() {
    if (original_) {
      setenv(name_, original_->c_str(), /*overwrite=*/1);
    } else {
      unsetenv(name_);
    }
  }

  void Set(const char* value) { setenv(name_, value, /*overwrite=*/1); }
  void Unset() { unsetenv(name_); }

 private:
  const char* name_;
  std::optional<std::string> original_;
};

TEST(OpenTelemetryMetricsTest, ResolveEndpointUsesDefaultWhenEnvIsUnset) {
  ScopedEnv metrics_endpoint{"OTEL_EXPORTER_OTLP_METRICS_ENDPOINT"};
  ScopedEnv endpoint{"OTEL_EXPORTER_OTLP_ENDPOINT"};
  metrics_endpoint.Unset();
  endpoint.Unset();

  EXPECT_EQ(OpenTelemetryMetrics::ResolveEndpoint(), "localhost:4317");
}

TEST(OpenTelemetryMetricsTest, ResolveEndpointPrefersMetricsEndpoint) {
  ScopedEnv metrics_endpoint{"OTEL_EXPORTER_OTLP_METRICS_ENDPOINT"};
  ScopedEnv endpoint{"OTEL_EXPORTER_OTLP_ENDPOINT"};
  metrics_endpoint.Set("metrics:4317");
  endpoint.Set("generic:4317");

  EXPECT_EQ(OpenTelemetryMetrics::ResolveEndpoint(), "metrics:4317");
}

TEST(OpenTelemetryMetricsTest, MeterRecordsValues) {
  OpenTelemetryMetrics runtime{OpenTelemetryMetricsOptions{
      .service_name = "scada-test",
      .export_interval = std::chrono::hours{1},
      .export_timeout = std::chrono::milliseconds{1},
      .endpoint = "localhost:4317"}};

  Meter meter{"scada.test"};
  MetricAttributes attributes{{"database_node_id", "ns=1;i=2"}};
  meter.AddCounter("scada.test.counter", 1, attributes);
  meter.AddUpDownCounter("scada.test.gauge", 1, attributes);
  meter.RecordHistogram("scada.test.duration_ms", 1.5, attributes);
}

TEST(OpenTelemetryMetricsTest, MeterRecordsValuesWithDefaultAttributes) {
  OpenTelemetryMetrics runtime{OpenTelemetryMetricsOptions{
      .service_name = "scada-test",
      .export_interval = std::chrono::hours{1},
      .export_timeout = std::chrono::milliseconds{1},
      .endpoint = "localhost:4317"}};

  Meter meter{"scada.test", {{"database_node_id", "ns=1;i=2"}}};
  meter.AddCounter("scada.test.counter", 1);
  meter.AddUpDownCounter("scada.test.gauge", 1);
  meter.RecordHistogram("scada.test.duration_ms", 1.5);
}

}  // namespace
}  // namespace metrics
