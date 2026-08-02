#pragma once

#include "base/lifetime.h"

#include <boost/asio/executor_work_guard.hpp>
#include <boost/asio/io_context.hpp>
#include <string>
#include <thread>

namespace scada::base {

// Names the calling thread for debuggers, crash dumps and profilers.
// Best-effort: a platform without support is a no-op, and a name longer than
// the platform allows is truncated rather than rejected.
void SetCurrentThreadName(const std::string& name);

// Owns one asio `io_context` and the single named thread that runs it.
//
// Anything that hands out an executor whose execution context is not already an
// `io_context` needs one of these. A bare `boost::asio::execution_context`
// looks like it avoids the thread, but it does not: asio creates its scheduler
// service there with `own_thread = true` and quietly spawns an unnamed thread
// of its own (`scheduler::scheduler(...)` in
// `boost/asio/detail/impl/scheduler.ipp`). Owning it here changes no runtime
// shape — it is the same one thread — but the thread has a name in a crash
// dump, and its shutdown is ordered by us instead of by service teardown.
//
// The thread runs only asio's reactor and whatever handlers are bound directly
// to this context. Completion handlers whose associated executor is elsewhere
// (the Qt loop, say) are dispatched there, not here.
class IoThread {
 public:
  explicit IoThread(std::string name);
  ~IoThread();

  IoThread(const IoThread&) = delete;
  IoThread& operator=(const IoThread&) = delete;

  boost::asio::io_context& context() SCADA_LIFETIME_BOUND { return context_; }

 private:
  boost::asio::io_context context_;
  // Keeps `run()` alive across idle periods, so the thread outlives the gaps
  // between one socket operation and the next.
  boost::asio::executor_work_guard<boost::asio::io_context::executor_type>
      work_guard_;
  std::thread thread_;
};

}  // namespace scada::base
