#include "metrics/tracing.h"

#include "metrics/trace_sink.h"

// TraceSpan

TraceSpan::TraceSpan(TraceSink* sink, TraceId trace_id, TraceId parent_trace_id)
    : sink_{sink}, trace_id_{trace_id} {
  if (sink_) {
    sink_->StartSpan(trace_id_, parent_trace_id);
  }
}

TraceSpan ::~TraceSpan() {
  if (sink_) {
    sink_->EndSpan(trace_id_);
  }
}
