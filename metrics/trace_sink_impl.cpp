#include "trace_sink_impl.h"

#include "base/boost_log.h"
#include "base/executor.h"
#include "metrics/tracing.h"

#include <mutex>
#include <unordered_set>

// TraceSinkImpl::Core

class TraceSinkImpl::Core : public std::enable_shared_from_this<Core> {
 public:
  Core(std::shared_ptr<Executor> executor, std::chrono::milliseconds timeout)
      : executor_{std::move(executor)}, timeout_{timeout} {}

  void StartSpan(const TraceSpanId& span_id, const TraceSpanId& parent_span_id);
  void EndSpan(const TraceSpanId& span_id);

 private:
  bool enabled() const { return timeout_ != std::chrono::milliseconds::zero(); }

  void OnTimeout(const TraceSpanId& span_id);

  BoostLogger logger_{LOG_NAME("Trace")};

  std::shared_ptr<Executor> executor_;
  std::chrono::milliseconds timeout_;

  std::mutex mutex_;
  std::unordered_set<TraceSpanId> active_span_ids_;
};

void TraceSinkImpl::Core::StartSpan(const TraceSpanId& span_id,
                                    const TraceSpanId& parent_span_id) {
  if (!enabled()) {
    return;
  }

  {
    std::lock_guard lock{mutex_};
    active_span_ids_.emplace(span_id);
  }

  executor_->PostDelayedTask(timeout_,
                             [this, ref = shared_from_this(), span_id] {
                               {
                                 std::lock_guard lock{mutex_};
                                 if (!active_span_ids_.erase(span_id)) {
                                   return;
                                 }
                               }

                               OnTimeout(span_id);
                             });
}

void TraceSinkImpl::Core::EndSpan(const TraceSpanId& span_id) {
  if (!enabled()) {
    return;
  }

  std::lock_guard lock{mutex_};
  active_span_ids_.erase(span_id);
}

void TraceSinkImpl::Core::OnTimeout(const TraceSpanId& span_id) {
  LOG_INFO(logger_) << "Span took too long" << LOG_TAG("SpanId", span_id);
}

// TraceSinkImpl

TraceSinkImpl::TraceSinkImpl(std::shared_ptr<Executor> executor,
                             std::chrono::milliseconds timeout)
    : core_{std::make_shared<Core>(std::move(executor), timeout)} {}

void TraceSinkImpl::StartSpan(const TraceSpanId& span_id,
                              const TraceSpanId& parent_span_id) {
  core_->StartSpan(span_id, parent_span_id);
}

void TraceSinkImpl::EndSpan(const TraceSpanId& span_id) {
  core_->EndSpan(span_id);
}
