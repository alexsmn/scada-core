#include "trace_sink_impl.h"

#include "base/any_executor.h"
#include "base/boost_log.h"
#include "base/check.h"
#include "metrics/tracing.h"

#include <boost/asio/steady_timer.hpp>
#include <memory>
#include <mutex>
#include <optional>
#include <unordered_map>
#include <vector>

// TraceSinkImpl::Core

class TraceSinkImpl::Core : public std::enable_shared_from_this<Core> {
 public:
  Core(AnyExecutor executor, std::chrono::milliseconds timeout)
      : executor_{std::move(executor)}, timeout_{timeout} {}

  void StartSpan(const TraceSpanId& span_id,
                 std::string_view name,
                 const TraceSpanId& parent_span_id);

  void EndSpan(const TraceSpanId& span_id);

  // Cancels every still-armed watchdog wait. Without this, spans that are
  // genuinely in flight at teardown keep the executor's io_context busy for up
  // to `timeout_` with nothing runnable, which stalls the shutdown drain.
  ~Core();

 private:
  struct SpanInfo {
    std::string name;
    TraceSpanId parent_id;
    // The armed watchdog. Held so EndSpan() (and ~Core) can cancel it instead
    // of leaving the wait outstanding until `timeout_` elapses.
    std::shared_ptr<boost::asio::steady_timer> timer;
  };

  bool enabled() const { return timeout_ != std::chrono::milliseconds::zero(); }

  void OnTimeout(const TraceSpanId& span_id, const SpanInfo& span);

  BoostLogger logger_{LOG_NAME("Trace")};

  AnyExecutor executor_;
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

  auto timer = std::make_shared<boost::asio::steady_timer>(executor_);

  {
    std::lock_guard lock{mutex_};
    scada::base::Check(!active_spans_.contains(span_id));
    active_spans_.try_emplace(span_id, std::string{name}, parent_span_id,
                              timer);
  }

  timer->expires_after(timeout_);
  // The handler holds only a weak reference. A strong one would keep the Core
  // alive until the watchdog expires, so ~Core -- the only thing that cancels
  // an armed wait -- could never run while a wait was pending: the wait kept
  // the Core alive and the Core was needed to cancel the wait. A watchdog is
  // also meaningless once the sink's owner is gone.
  timer->async_wait([weak_core = weak_from_this(), span_id,
                     timer](boost::system::error_code ec) {
    if (ec == boost::asio::error::operation_aborted)
      return;

    auto core = weak_core.lock();
    if (!core)
      return;

    std::optional<SpanInfo> timed_out_span;
    {
      std::lock_guard lock{core->mutex_};
      if (auto i = core->active_spans_.find(span_id);
          i != core->active_spans_.end()) {
        timed_out_span = std::move(i->second);
        core->active_spans_.erase(i);
      }
    }

    if (timed_out_span) {
      core->OnTimeout(span_id, *timed_out_span);
    }
  });
}

TraceSinkImpl::Core::~Core() {
  std::vector<std::shared_ptr<boost::asio::steady_timer>> timers;
  {
    std::lock_guard lock{mutex_};
    for (auto& [span_id, span] : active_spans_) {
      if (span.timer)
        timers.push_back(std::move(span.timer));
    }
    active_spans_.clear();
  }

  for (const auto& timer : timers)
    timer->cancel();
}

void TraceSinkImpl::Core::EndSpan(const TraceSpanId& span_id) {
  if (!enabled()) {
    return;
  }

  std::shared_ptr<boost::asio::steady_timer> timer;

  {
    std::lock_guard lock{mutex_};

    if (auto i = active_spans_.find(span_id); i != active_spans_.end()) {
      /*const SpanInfo& span = i->second;
      LOG_INFO(logger_) << "End span" << LOG_TAG("SpanId", span_id)
                        << LOG_TAG("SpanName", span.name)
                        << LOG_TAG("ParentSpanId", span.parent_id);*/
      timer = std::move(i->second.timer);
      active_spans_.erase(i);
    } else {
      LOG_INFO(logger_) << "End span after timeout"
                        << LOG_TAG("SpanId", span_id);
    }
  }

  // Cancel outside the lock: the completion handler takes `mutex_`, and asio
  // may invoke it inline on this thread. A span that has ended must not leave
  // its watchdog armed — otherwise every completed span holds outstanding
  // io_context work for the full timeout.
  if (timer)
    timer->cancel();
}

void TraceSinkImpl::Core::OnTimeout(const TraceSpanId& span_id,
                                    const SpanInfo& span) {
  LOG_WARNING(logger_) << "Span took too long" << LOG_TAG("SpanId", span_id)
                       << LOG_TAG("SpanName", span.name)
                       << LOG_TAG("ParentSpanId", span.parent_id);
}

// TraceSinkImpl

TraceSinkImpl::TraceSinkImpl(AnyExecutor executor,
                             std::chrono::milliseconds timeout)
    : core_{std::make_shared<Core>(std::move(executor), timeout)} {}

void TraceSinkImpl::StartSpan(const TraceSpanId& span_id,
                              std::string_view name,
                              const TraceSpanId& parent_span_id,
                              TraceSpanKind kind,
                              std::string_view remote_parent) {
  core_->StartSpan(span_id, name, parent_span_id);
}

void TraceSinkImpl::EndSpan(const TraceSpanId& span_id) {
  core_->EndSpan(span_id);
}
