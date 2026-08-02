#include "base/io_thread.h"

#include <boost/asio/executor_work_guard.hpp>
#include <utility>

#if defined(_WIN32)
#include <windows.h>
// processthreadsapi.h must follow windows.h.
#include <processthreadsapi.h>
#elif defined(__APPLE__) || defined(__linux__)
#include <pthread.h>
#endif

namespace scada::base {

void SetCurrentThreadName(const std::string& name) {
#if defined(_WIN32)
  const std::wstring wide{name.begin(), name.end()};
  SetThreadDescription(GetCurrentThread(), wide.c_str());
#elif defined(__APPLE__)
  // Darwin names the calling thread only, and takes the name by value.
  pthread_setname_np(name.c_str());
#elif defined(__linux__)
  // Linux caps the name at 16 bytes including the terminator and *rejects* a
  // longer one outright rather than truncating, so truncate here or the name
  // is silently lost.
  pthread_setname_np(pthread_self(), name.substr(0, 15).c_str());
#else
  (void)name;
#endif
}

IoThread::IoThread(std::string name)
    : work_guard_{boost::asio::make_work_guard(context_)} {
  // Started in the body, not the member initialiser list: the thread touches
  // context_ and work_guard_, and both must be fully constructed first.
  thread_ = std::thread{[this, name = std::move(name)] {
    SetCurrentThreadName(name);
    context_.run();
  }};
}

IoThread::~IoThread() {
  // Drop the guard first so `run()` may return once the queue drains, then stop
  // outright. Waiting for a natural drain would block on whatever is still
  // armed — a reconnect timer, a socket read that will never complete — and a
  // client being torn down must not wait on the network to exit.
  work_guard_.reset();
  context_.stop();
  if (thread_.joinable())
    thread_.join();
}

}  // namespace scada::base
