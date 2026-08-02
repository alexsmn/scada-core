#pragma once

#include "metrics/tracing.h"

// Similar to OpenTelemetry's Tracer:
// https://opentelemetry-cpp.readthedocs.io/en/latest/otel_docs/classopentelemetry_1_1trace_1_1Tracer.html#class-tracer
class Tracer {
 public:
  explicit Tracer(TraceSink& sink) : sink_{sink} {}

  TraceSpan StartSpan(std::string_view name);

  // Starts a span with an explicit kind, optionally continuing a trace from
  // another process: `remote_parent` is typically
  // `scada::ServiceContext::trace_id()` and is honored when it is a valid W3C
  // traceparent (anything else — legacy UUIDs, empty — starts a new root).
  TraceSpan StartSpan(std::string_view name,
                      TraceSpanKind kind,
                      std::string_view remote_parent);

  static Tracer& None();

 private:
  TraceSink& sink_;
};
