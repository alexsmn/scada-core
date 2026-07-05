#pragma once

#include "metrics/trace_sink.h"

#include <opentelemetry/nostd/shared_ptr.h>
#include <opentelemetry/trace/span.h>
#include <opentelemetry/trace/tracer.h>

#include <mutex>
#include <unordered_map>

namespace metrics {

// Bridges the SCADA `TraceSink` contract to OpenTelemetry spans: every live
// SCADA span id maps to an OTel span created on the given tracer. Parent
// resolution per StartSpan: a live local `parent_span_id` wins; otherwise a
// valid W3C `remote_parent` continues the remote trace; otherwise the span
// is a new root. Thread-safe — spans start and end on arbitrary executor
// threads.
class OtelTraceSink final : public TraceSink {
 public:
  explicit OtelTraceSink(
      opentelemetry::nostd::shared_ptr<opentelemetry::trace::Tracer> tracer);
  ~OtelTraceSink() override;

  OtelTraceSink(const OtelTraceSink&) = delete;
  OtelTraceSink& operator=(const OtelTraceSink&) = delete;

  // TraceSink
  void StartSpan(const TraceSpanId& span_id,
                 std::string_view name,
                 const TraceSpanId& parent_span_id,
                 TraceSpanKind kind,
                 std::string_view remote_parent) override;
  void EndSpan(const TraceSpanId& span_id) override;
  std::string GetTraceParent(const TraceSpanId& span_id) const override;

 private:
  using SpanPtr = opentelemetry::nostd::shared_ptr<opentelemetry::trace::Span>;

  const opentelemetry::nostd::shared_ptr<opentelemetry::trace::Tracer> tracer_;

  mutable std::mutex mutex_;
  // Live spans by SCADA span id. Guarded by `mutex_`.
  std::unordered_map<TraceSpanId, SpanPtr> spans_;
};

}  // namespace metrics
