#include "metrics/tracing.h"

#include "metrics/trace_sink.h"

// TraceSpan

TraceSpan::TraceSpan(TraceSink* sink,
                     const TraceSpanId& span_id,
                     const TraceSpanId& parent_span_id)
    : sink_{sink}, span_id_{span_id} {
  if (sink_) {
    sink_->StartSpan(span_id_, parent_span_id);
  }
}

TraceSpan::~TraceSpan() {
  if (sink_) {
    sink_->EndSpan(span_id_);
  }
}
