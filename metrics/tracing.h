#pragma once

#include <cstdint>

class Tracer;

using TraceId = int64_t;

inline TraceId GenerateTraceId() {
  int64_t next_trace_id = 1;
  return next_trace_id++;
}

class TraceSink {
 public:
  virtual ~TraceSink() = default;

  virtual void Start(const TraceId& trace_id) = 0;
  virtual void Finish(const TraceId& trace_id) = 0;
};

class NoTraceSink final : public TraceSink {
 public:
  void Start(const TraceId& trace_id) override {}
  void Finish(const TraceId& trace_id) override {}
};

class TraceScope {
 public:
  ~TraceScope() { sink_.Finish(trace_id_); }

 private:
  TraceScope(TraceSink& sink, const TraceId& trace_id)
      : sink_{sink}, trace_id_{trace_id} {}

  TraceSink& sink_;
  const TraceId trace_id_;

  friend class Tracer;
};

class Tracer {
 public:
  explicit Tracer(TraceSink& sink) : sink_{sink} {}

  TraceScope Start() { return Start(GenerateTraceId()); }

  TraceScope Start(const TraceId& trace_id) {
    sink_.Start(trace_id);
    return TraceScope{sink_, trace_id};
  }

 private:
  TraceSink& sink_;
};
