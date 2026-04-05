#include "trace_sink_impl.h"

#include "base/boost_log.h"
#include "base/executor.h"
#include "metrics/tracing.h"

#include <mutex>
#include <unordered_map>

// TraceSinkImpl::Core

class TraceSinkImpl::Core : public std::enable_shared_from_this<Core> {
 public:
  Core(std::shared_ptr<Executor> executor, std::chrono::milliseconds timeout)
      : executor_{std::move(executor)}, timeout_{timeout} {}

  void StartSpan(const TraceSpanId& span_id,
                 std::string_view name,
                 const TraceSpanId& parent_span_id);

  void EndSpan(const TraceSpanId& span_id);

 private:
  struct SpanInfo {
    std::string name;
    TraceSpanId parent_id;
  };

  bool enabled() const { return timeout_ != std::chrono::milliseconds::zero(); }

  void OnTimeout(const TraceSpanId& span_id, const SpanInfo& span);

  BoostLogger logger_{LOG_NAME("Trace")};

  std::shared_ptr<Executor> executor_;
  std::chrono::milliseconds timeout_;

  std::mutex mutex_;
  std::unordered_map<TraceSpanId, SpanInfo> active_spans_;
};

void TraceSinkImpl::Core::StartSpan(const TraceSpanId& span_id,
                                    std::string_view name,
                                    const TraceSpanId& parent_span_id) {
  if (!enabled()) {
    return;
  }

  /*LOG_INFO(logger_) << "Start span"
                         << LOG_TAG("SpanName", std::string{name})
                    << LOG_TAG("SpanId", span_id)
                    << LOG_TAG("ParentSpanId", parent_span_id);*/

  {
    std::lock_guard lock{mutex_};
    assert(!active_spans_.contains(span_id));
    active_spans_.try_emplace(span_id, std::string{name}, parent_span_id);
  }

  executor_->PostDelayedTask(
      timeout_, [this, ref = shared_from_this(), span_id] {
        std::optional<SpanInfo> timed_out_span;
        {
          std::lock_guard lock{mutex_};
          if (auto i = active_spans_.find(span_id); i != active_spans_.end()) {
            timed_out_span = std::move(i->second);
            active_spans_.erase(i);
          }
        }

        if (timed_out_span) {
          OnTimeout(span_id, *timed_out_span);
        }
      });
}

void TraceSinkImpl::Core::EndSpan(const TraceSpanId& span_id) {
  if (!enabled()) {
    return;
  }

  std::lock_guard lock{mutex_};

  if (auto i = active_spans_.find(span_id); i != active_spans_.end()) {
    /*const SpanInfo& span = i->second;
    LOG_INFO(logger_) << "End span" << LOG_TAG("SpanId", span_id)
                      << LOG_TAG("SpanName", span.name)
                      << LOG_TAG("ParentSpanId", span.parent_id);*/
    active_spans_.erase(i);
  } else {
    LOG_INFO(logger_) << "End span after timeout" << LOG_TAG("SpanId", span_id);
  }
}

void TraceSinkImpl::Core::OnTimeout(const TraceSpanId& span_id,
                                    const SpanInfo& span) {
  LOG_WARNING(logger_) << "Span took too long" << LOG_TAG("SpanId", span_id)
                       << LOG_TAG("SpanName", span.name)
                       << LOG_TAG("ParentSpanId", span.parent_id);
}

// TraceSinkImpl

TraceSinkImpl::TraceSinkImpl(std::shared_ptr<Executor> executor,
                             std::chrono::milliseconds timeout)
    : core_{std::make_shared<Core>(std::move(executor), timeout)} {}

void TraceSinkImpl::StartSpan(const TraceSpanId& span_id,
                              std::string_view name,
                              const TraceSpanId& parent_span_id) {
  core_->StartSpan(span_id, name, parent_span_id);
}

void TraceSinkImpl::EndSpan(const TraceSpanId& span_id) {
  core_->EndSpan(span_id);
}
