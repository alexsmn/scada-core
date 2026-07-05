#include "metrics/tracer.h"

#include "metrics/trace_sink.h"

// static
Tracer& Tracer::None() {
  static NoTraceSink sink;
  static Tracer tracer{sink};
  return tracer;
}

TraceSpan Tracer::StartSpan(std::string_view name) {
  return TraceSpan{sink_, name};
}

TraceSpan Tracer::StartSpan(std::string_view name,
                            TraceSpanKind kind,
                            std::string_view remote_parent) {
  return TraceSpan{sink_, name, kind, remote_parent};
}
