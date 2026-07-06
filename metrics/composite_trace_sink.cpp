#include "metrics/composite_trace_sink.h"

#include "base/check.h"

namespace metrics {

CompositeTraceSink::CompositeTraceSink(std::vector<TraceSink*> sinks)
    : sinks_{std::move(sinks)} {
  for (TraceSink* sink : sinks_) {
    base::Check(sink);
  }
}

void CompositeTraceSink::StartSpan(const TraceSpanId& span_id,
                                   std::string_view name,
                                   const TraceSpanId& parent_span_id,
                                   TraceSpanKind kind,
                                   std::string_view remote_parent) {
  for (TraceSink* sink : sinks_) {
    sink->StartSpan(span_id, name, parent_span_id, kind, remote_parent);
  }
}

void CompositeTraceSink::EndSpan(const TraceSpanId& span_id) {
  for (TraceSink* sink : sinks_) {
    sink->EndSpan(span_id);
  }
}

void CompositeTraceSink::SetSpanAttribute(const TraceSpanId& span_id,
                                          std::string_view key,
                                          std::string_view value) {
  for (TraceSink* sink : sinks_) {
    sink->SetSpanAttribute(span_id, key, value);
  }
}

std::string CompositeTraceSink::GetTraceParent(
    const TraceSpanId& span_id) const {
  for (TraceSink* sink : sinks_) {
    std::string trace_parent = sink->GetTraceParent(span_id);
    if (!trace_parent.empty()) {
      return trace_parent;
    }
  }
  return {};
}

}  // namespace metrics
