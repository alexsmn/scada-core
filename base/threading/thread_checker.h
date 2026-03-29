#pragma once

#include <cassert>
#include <thread>

#ifndef NDEBUG

class ThreadChecker {
 public:
  ThreadChecker() : thread_id_(std::this_thread::get_id()) {}
  bool CalledOnValidThread() const {
    return thread_id_ == std::this_thread::get_id();
  }
  void DetachFromThread() { thread_id_ = std::thread::id{}; }

 private:
  mutable std::thread::id thread_id_;
};

#define THREAD_CHECKER(name) ThreadChecker name
#define DCHECK_CALLED_ON_VALID_THREAD(name) assert((name).CalledOnValidThread())
#define DETACH_FROM_THREAD(name) (name).DetachFromThread()

#else  // NDEBUG

#define THREAD_CHECKER(name)
#define DCHECK_CALLED_ON_VALID_THREAD(name) ((void)0)
#define DETACH_FROM_THREAD(name) ((void)0)

#endif
