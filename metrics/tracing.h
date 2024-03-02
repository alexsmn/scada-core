#pragma once

#include "metrics/trace_id.h"

class TraceSink;

class TraceSpan {
 public:
  TraceSpan()
      : TraceSpan{nullptr, GenerateTraceId(), /*parent_trace_id=*/kNoTraceId} {}

  explicit TraceSpan(TraceSink& sink)
      : TraceSpan{&sink, GenerateTraceId(), /*parent_trace_id=*/kNoTraceId} {}

  ~TraceSpan();

  TraceSpan(TraceSpan&& other) noexcept
      : sink_{other.sink_}, trace_id_{other.trace_id_} {
    other.sink_ = nullptr;
  }

  TraceSpan& operator=(TraceSpan&& other) noexcept {
    if (this != &other) {
      sink_ = other.sink_;
      trace_id_ = other.trace_id_;
      other.sink_ = nullptr;
    }
    return *this;
  }

  TraceSpan StartSpan() const {
    return TraceSpan{sink_, GenerateTraceId(), /*parent_trace_id=*/trace_id_};
  }

 private:
  TraceSpan(TraceSink* sink, TraceId trace_id, TraceId parent_trace_id);

  TraceSink* sink_ = nullptr;
  TraceId trace_id_ = 0;
};
