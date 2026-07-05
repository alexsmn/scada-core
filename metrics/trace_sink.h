#pragma once

#include "metrics/trace_id.h"
#include "metrics/trace_span_kind.h"

#include <string>
#include <string_view>

// Receives span lifecycle events from `TraceSpan`/`Tracer`. Implementations:
// the hung-span watchdog (`TraceSinkImpl`), the OpenTelemetry export bridge
// (`OtelTraceSink`), and their fan-out (`CompositeTraceSink`).
class TraceSink {
 public:
  virtual ~TraceSink() = default;

  // `parent_span_id` links to a live local span (empty = none).
  // `remote_parent` optionally carries a W3C traceparent from another process;
  // sinks that don't understand it ignore it. When both are given the local
  // parent wins.
  virtual void StartSpan(const TraceSpanId& span_id,
                         std::string_view name,
                         const TraceSpanId& parent_span_id,
                         TraceSpanKind kind,
                         std::string_view remote_parent) = 0;

  virtual void EndSpan(const TraceSpanId& span_id) = 0;

  // The W3C traceparent of a live span, for injecting into outbound requests.
  // Empty when the sink does not export spans (watchdog-only configurations)
  // or the span is unknown.
  virtual std::string GetTraceParent(const TraceSpanId& span_id) const {
    return {};
  }
};

class NoTraceSink final : public TraceSink {
 public:
  void StartSpan(const TraceSpanId& span_id,
                 std::string_view name,
                 const TraceSpanId& parent_span_id,
                 TraceSpanKind kind,
                 std::string_view remote_parent) override {}

  void EndSpan(const TraceSpanId& span_id) override {}
};
