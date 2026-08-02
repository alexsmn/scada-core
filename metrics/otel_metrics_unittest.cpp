#include "metrics/otel_metrics.h"

#include <opentelemetry/sdk/common/global_log_handler.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <string>
#include <vector>

namespace scada::metrics {
namespace {

namespace internal_log = opentelemetry::sdk::common::internal_log;

// Captures what the OpenTelemetry SDK writes to its own internal log, which is
// where the exporter reports configuration failures. Asserting on it is how a
// test can see the errors an operator sees on the console.
class CapturingLogHandler final : public internal_log::LogHandler {
 public:
  void Handle(internal_log::LogLevel level,
              const char* file,
              int line,
              const char* msg,
              const opentelemetry::sdk::common::AttributeMap&
                  attributes) noexcept override {
    if (level == internal_log::LogLevel::Error)
      errors_.emplace_back(msg ? msg : "");
  }

  const std::vector<std::string>& errors() const { return errors_; }

 private:
  std::vector<std::string> errors_;
};

// Installs the capturing handler for the duration of a test and restores the
// SDK's default afterwards, so one test's handler cannot outlive it.
class ScopedLogCapture {
 public:
  ScopedLogCapture() : handler_{new CapturingLogHandler} {
    previous_level_ = internal_log::GlobalLogHandler::GetLogLevel();
    internal_log::GlobalLogHandler::SetLogLevel(internal_log::LogLevel::Error);
    internal_log::GlobalLogHandler::SetLogHandler(handler_);
  }

  ~ScopedLogCapture() {
    internal_log::GlobalLogHandler::SetLogHandler(
        opentelemetry::nostd::shared_ptr<internal_log::LogHandler>{
            new internal_log::DefaultLogHandler});
    internal_log::GlobalLogHandler::SetLogLevel(previous_level_);
  }

  const std::vector<std::string>& errors() const {
    return static_cast<const CapturingLogHandler&>(*handler_).errors();
  }

 private:
  opentelemetry::nostd::shared_ptr<internal_log::LogHandler> handler_;
  internal_log::LogLevel previous_level_;
};

TEST(OpenTelemetryMetricsTest, MeterRecordsValues) {
  OpenTelemetryMetrics runtime{OpenTelemetryMetricsOptions{
      .service_name = "scada-test",
      .export_interval = std::chrono::hours{1},
      .export_timeout = std::chrono::milliseconds{1},
      .endpoint = "localhost:4317"}};

  Meter meter{"scada.test"};
  MetricAttributes attributes{{"node_id", "ns=1;i=2"}};
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

  Meter meter{"scada.test", {{"node_id", "ns=1;i=2"}}};
  meter.AddCounter("scada.test.counter", 1);
  meter.AddUpDownCounter("scada.test.gauge", 1);
  meter.RecordHistogram("scada.test.duration_ms", 1.5);
}

// Regression: the OTLP exporter used to be built even when no endpoint was
// configured. The OTLP client cannot create a gRPC stub for an empty target, so
// every process started without `--otlp-endpoint` logged an "empty endpoint"
// error at startup and then "service stub unavailable" once per export
// interval, forever. Configuring no endpoint must be silent.
TEST(OpenTelemetryMetricsTest, EmptyEndpointReportsNoExporterErrors) {
  ScopedLogCapture log_capture;

  OpenTelemetryMetrics runtime{OpenTelemetryMetricsOptions{
      .service_name = "scada-test",
      .export_interval = std::chrono::hours{1},
      .export_timeout = std::chrono::milliseconds{1},
      .endpoint = ""}};

  EXPECT_THAT(log_capture.errors(), ::testing::IsEmpty());
}

// Local metric views are fed by the in-memory reader, which must survive the
// absence of an OTLP endpoint — the point is to drop the export, not the
// metrics.
TEST(OpenTelemetryMetricsTest, EmptyEndpointStillRecordsInMemoryValues) {
  OpenTelemetryMetrics runtime{OpenTelemetryMetricsOptions{
      .service_name = "scada-test",
      .export_interval = std::chrono::hours{1},
      .export_timeout = std::chrono::milliseconds{1},
      .endpoint = ""}};

  Meter meter{"scada.test"};
  meter.AddCounter("scada.test.counter", 1, {{"node_id", "ns=1;i=2"}});
}

}  // namespace
}  // namespace scada::metrics
