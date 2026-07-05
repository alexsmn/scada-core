#pragma once

#include "base/any_executor.h"
#include "metrics/trace_sink.h"

#include <chrono>
#include <memory>

class TraceSinkImpl final : public TraceSink {
 public:
  TraceSinkImpl(AnyExecutor executor, std::chrono::milliseconds timeout);

  // TraceSink. The watchdog only tracks span liveness; `kind` and
  // `remote_parent` are ignored.
  void StartSpan(const TraceSpanId& span_id,
                 std::string_view name,
                 const TraceSpanId& parent_span_id,
                 TraceSpanKind kind,
                 std::string_view remote_parent) override;
  void EndSpan(const TraceSpanId& span_id) override;

 private:
  class Core;

  const std::shared_ptr<Core> core_;
};
