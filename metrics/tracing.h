#pragma once

#include "metrics/trace_id.h"

class TraceSink;

class [[nodiscard]] TraceSpan {
 public:
  TraceSpan()
      : TraceSpan{nullptr, GenerateTraceSpanId(), /*parent_span_id=*/{}} {}

  explicit TraceSpan(TraceSink& sink)
      : TraceSpan{&sink, GenerateTraceSpanId(), /*parent_span_id=*/{}} {}

  ~TraceSpan();

  TraceSpan(TraceSpan&& other) noexcept
      : sink_{other.sink_}, span_id_{other.span_id_} {
    other.sink_ = nullptr;
  }

  TraceSpan& operator=(TraceSpan&& other) noexcept {
    if (this != &other) {
      sink_ = other.sink_;
      span_id_ = other.span_id_;
      other.sink_ = nullptr;
    }
    return *this;
  }

  const TraceSpanId& span_id() const { return span_id_; }

  TraceSpan StartSpan() const { return StartSpan(GenerateTraceSpanId()); }

  TraceSpan StartSpan(const TraceSpanId& span_id) const {
    return TraceSpan{sink_, span_id, /*parent_span_id=*/span_id_};
  }

 private:
  TraceSpan(TraceSink* sink,
            const TraceSpanId& span_id,
            const TraceSpanId& parent_span_id);

  TraceSink* sink_ = nullptr;
  TraceSpanId span_id_;
};
