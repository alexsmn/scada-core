#pragma once

#include <atomic>
#include <source_location>
#include <thread>

#include "base/check.h"

// Verifies that methods of a class are called on the thread the checker was
// bound to. Bound to the constructing thread; DetachFromThread() unbinds it,
// and the next thread that performs a check becomes the new bound thread.
//
// Compiled and enforced in all build types: a thread-affinity violation in
// production is state corruption and must fail-stop rather than proceed.
class ThreadChecker {
 public:
  ThreadChecker() : thread_id_(std::this_thread::get_id()) {}

  // Returns true if called on the bound thread. After DetachFromThread(),
  // the first calling thread rebinds the checker and passes.
  bool CalledOnValidThread() const {
    const std::thread::id current = std::this_thread::get_id();
    // Relaxed ordering is sufficient: the checker only guards single-thread
    // affinity, so a concurrent rebind race can only happen when the checked
    // invariant is already violated.
    std::thread::id bound = thread_id_.load(std::memory_order_relaxed);
    if (bound == std::thread::id{}) {
      if (thread_id_.compare_exchange_strong(bound, current,
                                             std::memory_order_relaxed)) {
        return true;
      }
      // Another thread won the rebind; `bound` now holds its id.
    }
    return bound == current;
  }

  // Panics unless called on the bound thread.
  void CheckCalledOnValidThread(const std::source_location& location =
                                    std::source_location::current()) const {
    base::Check(CalledOnValidThread(), "Called on invalid thread", location);
  }

  // Unbinds the checker; the next thread that checks becomes the bound one.
  void DetachFromThread() {
    thread_id_.store(std::thread::id{}, std::memory_order_relaxed);
  }

 private:
  mutable std::atomic<std::thread::id> thread_id_;
};
