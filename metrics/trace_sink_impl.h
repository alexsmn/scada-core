#pragma once

#include "metrics/tracing.h"

#include <chrono>
#include <memory>

class Executor;

class TraceSinkImpl final : public TraceSink {
 public:
  TraceSinkImpl(std::shared_ptr<Executor> executor,
                std::chrono::milliseconds timeout);

  // TraceSink
  void Start(const TraceId& trace_id) override;
  void Finish(const TraceId& trace_id) override;

 private:
  class Core;

  const std::shared_ptr<Core> core_;
};