#pragma once

#include "metrics/tracing.h"

// Similar to OpenTelemetry's Tracer:
// https://opentelemetry-cpp.readthedocs.io/en/latest/otel_docs/classopentelemetry_1_1trace_1_1Tracer.html#class-tracer
class Tracer {
 public:
  explicit Tracer(TraceSink& sink) : sink_{sink} {}

  TraceSpan StartSpan(std::string_view name);

  static Tracer& None();

 private:
  TraceSink& sink_;
};