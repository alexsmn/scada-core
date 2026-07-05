#pragma once

#include "metrics/trace_sink.h"

#include <vector>

namespace metrics {

// Fans span events out to a fixed set of sinks (e.g. the hung-span watchdog
// plus the OpenTelemetry export bridge). The sink set is established at
// construction and must outlive this object; there is no dynamic
// registration, so no synchronization is needed here.
class CompositeTraceSink final : public TraceSink {
 public:
  explicit CompositeTraceSink(std::vector<TraceSink*> sinks);

  // TraceSink
  void StartSpan(const TraceSpanId& span_id,
                 std::string_view name,
                 const TraceSpanId& parent_span_id,
                 TraceSpanKind kind,
                 std::string_view remote_parent) override;
  void EndSpan(const TraceSpanId& span_id) override;
  // First non-empty result wins (only the exporting sink produces one).
  std::string GetTraceParent(const TraceSpanId& span_id) const override;

 private:
  const std::vector<TraceSink*> sinks_;
};

}  // namespace metrics
