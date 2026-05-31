#include "metrics/otel_metrics.h"

#include <gtest/gtest.h>

namespace metrics {
namespace {

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
