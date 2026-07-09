#pragma once

#include "base/lifetime.h"
#include "metrics/trace_id.h"
#include "metrics/trace_span_kind.h"

#include <string>
#include <string_view>

class TraceSink;

class [[nodiscard]] TraceSpan {
 public:
  TraceSpan(TraceSink& sink, std::string_view name);
  TraceSpan(TraceSink& sink,
            std::string_view name,
            TraceSpanKind kind,
            std::string_view remote_parent);
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

  const TraceSpanId& span_id() const SCADA_LIFETIME_BOUND { return span_id_; }

  // The span's W3C traceparent, for injecting into outbound requests (via
  // `scada::ServiceContext::with_trace_id`). Empty when no exporting sink is
  // configured or the span was moved out.
  std::string traceparent() const;

  // Attaches a string attribute (request parameters such as node ids) to the
  // span. No-op when no exporting sink is configured or the span was moved
  // out.
  void SetAttribute(std::string_view key, std::string_view value);

  TraceSpan StartSpan(std::string_view name) const;

 private:
  TraceSpan(TraceSink& sink,
            const TraceSpanId& span_id,
            std::string_view name,
            const TraceSpanId& parent_span_id,
            TraceSpanKind kind,
            std::string_view remote_parent);

  // Null when span is moved out.
  TraceSink* sink_ = nullptr;

  TraceSpanId span_id_;
};
