#pragma once

#include "metrics/tracing.h"

class Tracer {
 public:
  TraceSpan OpenSpan(const TraceSpanId& span_id);
};