#pragma once

#include "metrics/trace_sink.h"

#include <chrono>
#include <memory>

class Executor;

class TraceSinkImpl final : public TraceSink {
 public:
  TraceSinkImpl(std::shared_ptr<Executor> executor,
                std::chrono::milliseconds timeout);

  // TraceSink
  void StartSpan(TraceId trace_id, TraceId parent_trace_id) override;
  void EndSpan(TraceId trace_id) override;

 private:
  class Core;

  const std::shared_ptr<Core> core_;
};