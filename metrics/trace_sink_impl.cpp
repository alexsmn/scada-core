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

  void Start(const TraceId& trace_id);
  void Finish(const TraceId& trace_id);

 private:
  bool enabled() const { return timeout_ != std::chrono::milliseconds::zero(); }

  void OnTimeout(const TraceId& trace_id);

  BoostLogger logger_{LOG_NAME("Trace")};

  std::shared_ptr<Executor> executor_;
  std::chrono::milliseconds timeout_;

  std::mutex mutex_;
  std::unordered_set<TraceId> active_trace_ids_;
};

void TraceSinkImpl::Core::Start(const TraceId& trace_id) {
  if (!enabled()) {
    return;
  }

  {
    std::lock_guard lock{mutex_};
    active_trace_ids_.emplace(trace_id);
  }

  executor_->PostDelayedTask(timeout_,
                             [this, ref = shared_from_this(), trace_id] {
                               {
                                 std::lock_guard lock{mutex_};
                                 if (!active_trace_ids_.erase(trace_id)) {
                                   return;
                                 }
                               }

                               OnTimeout(trace_id);
                             });
}

void TraceSinkImpl::Core::Finish(const TraceId& trace_id) {
  if (!enabled()) {
    return;
  }

  std::lock_guard lock{mutex_};
  active_trace_ids_.erase(trace_id);
}

void TraceSinkImpl::Core::OnTimeout(const TraceId& trace_id) {
  LOG_INFO(logger_) << "Scope took too long" << LOG_TAG("TraceId", trace_id);
}

// TraceSinkImpl

TraceSinkImpl::TraceSinkImpl(std::shared_ptr<Executor> executor,
                             std::chrono::milliseconds timeout)
    : core_{std::make_shared<Core>(std::move(executor), timeout)} {}

void TraceSinkImpl::Start(const TraceId& trace_id) {
  core_->Start(trace_id);
}

void TraceSinkImpl::Finish(const TraceId& trace_id) {
  core_->Finish(trace_id);
}
