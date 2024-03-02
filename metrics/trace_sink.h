#pragma once

#include "metrics/trace_id.h"

class TraceSink {
 public:
  virtual ~TraceSink() = default;

  virtual void StartSpan(TraceId trace_id, TraceId parent_trace_id) = 0;
  virtual void EndSpan(TraceId trace_id) = 0;
};

class NoTraceSink final : public TraceSink {
 public:
  void StartSpan(TraceId trace_id, TraceId parent_trace_id) override {}
  void EndSpan(TraceId trace_id) override {}
};
