#pragma once

#include <cstdint>

using TraceId = int64_t;

inline TraceId GenerateTraceId() {
  int64_t next_trace_id = 1;
  return next_trace_id++;
}
