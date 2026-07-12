#pragma once

#include <opentelemetry/logs/logger.h>
#include <opentelemetry/nostd/shared_ptr.h>
#include <opentelemetry/sdk/logs/exporter.h>

#include <memory>
#include <string>

#include "metrics/metrics_compat.h"
namespace scada::metrics {

// Defines OpenTelemetry log exporter and resource settings for the process.
// `service_name` and `endpoint` follow the traces/metrics convention — one
// OTLP destination and one resource identity per tier.
struct OpenTelemetryLogsOptions {
  std::string service_name;
  std::string endpoint;
};

// Owns the OpenTelemetry logger provider: OTLP/gRPC log-record exporter
// behind a batch log-record processor. Like the traces runtime it does NOT
// install a global provider — consumers hold the logger via logger(), which
// keeps tests hermetic. Pair with ScopedOtelLogSink (otel_log_sink.h) to
// bridge Boost.Log records into this pipeline.
class OpenTelemetryLogs {
 public:
  explicit OpenTelemetryLogs(OpenTelemetryLogsOptions options);
  // Test seam: exports through the given exporter instead of OTLP/gRPC.
  OpenTelemetryLogs(
      OpenTelemetryLogsOptions options,
      std::unique_ptr<opentelemetry::sdk::logs::LogRecordExporter> exporter);
  ~OpenTelemetryLogs();

  OpenTelemetryLogs(const OpenTelemetryLogs&) = delete;
  OpenTelemetryLogs& operator=(const OpenTelemetryLogs&) = delete;

  opentelemetry::nostd::shared_ptr<opentelemetry::logs::Logger> logger();

  // Flushes buffered log records through the processor synchronously.
  // Records are only handed to the exporter on batch boundaries; tests (and
  // shutdown) use this to observe them deterministically.
  void ForceFlush();

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

}  // namespace scada::metrics
