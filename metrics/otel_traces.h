#pragma once

#include <opentelemetry/nostd/shared_ptr.h>
#include <opentelemetry/sdk/trace/exporter.h>
#include <opentelemetry/trace/tracer.h>

#include <memory>
#include <string>

namespace metrics {

// Defines OpenTelemetry span exporter and resource settings for the process.
// `service_name` and `endpoint` are shared with the metrics runtime — one
// OTLP destination and one resource identity per tier.
struct OpenTelemetryTracesOptions {
  std::string service_name;
  std::string endpoint;
  // ParentBased(TraceIdRatioBased) root sampling ratio in [0, 1]. Downstream
  // tiers honor the caller's sampling decision from the propagated
  // traceparent flags; the ratio only applies to new roots.
  double sampling_ratio = 1.0;
};

// Owns the OpenTelemetry tracer provider: OTLP/gRPC span exporter behind a
// batch span processor with a parent-based ratio sampler. Unlike the metrics
// runtime it does NOT install a global provider — consumers hold the tracer
// via tracer(), which keeps tests hermetic.
class OpenTelemetryTraces {
 public:
  explicit OpenTelemetryTraces(OpenTelemetryTracesOptions options);
  // Test seam: exports through the given exporter instead of OTLP/gRPC.
  OpenTelemetryTraces(
      OpenTelemetryTracesOptions options,
      std::unique_ptr<opentelemetry::sdk::trace::SpanExporter> exporter);
  ~OpenTelemetryTraces();

  OpenTelemetryTraces(const OpenTelemetryTraces&) = delete;
  OpenTelemetryTraces& operator=(const OpenTelemetryTraces&) = delete;

  opentelemetry::nostd::shared_ptr<opentelemetry::trace::Tracer> tracer();

  // Flushes buffered spans through the processor synchronously. Ended spans
  // are only handed to the exporter on batch boundaries; tests (and shutdown)
  // use this to observe them deterministically.
  void ForceFlush();

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

}  // namespace metrics
