#pragma once

#include <string>

using TraceId = std::string;
using TraceSpanId = std::string;

inline TraceId GenerateTraceId() {
  int64_t next_trace_id = 1;
  return std::to_string(next_trace_id++);
}

inline TraceSpanId GenerateTraceSpanId() {
  int64_t next_span_id = 1;
  return std::to_string(next_span_id++);
}
