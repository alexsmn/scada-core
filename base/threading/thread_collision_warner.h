#pragma once

#include <atomic>
#include <cassert>
#include <thread>

#ifndef NDEBUG

class ThreadCollisionWarner {
 public:
  void EnterSelf() {
    auto expected = std::thread::id{};
    auto current = std::this_thread::get_id();
    assert(owner_.compare_exchange_strong(expected, current) ||
           expected == current);
  }
  void LeaveSelf() { owner_.store(std::thread::id{}); }
  void EnterSelfRecursive() {
    auto expected = std::thread::id{};
    auto current = std::this_thread::get_id();
    if (owner_.compare_exchange_strong(expected, current) ||
        expected == current) {
      ++depth_;
    } else {
      assert(false && "ThreadCollisionWarner: concurrent access detected");
    }
  }
  void LeaveSelfRecursive() {
    if (--depth_ == 0)
      owner_.store(std::thread::id{});
  }

 private:
  std::atomic<std::thread::id> owner_{};
  int depth_ = 0;
};

class ScopedCollisionWarner {
 public:
  explicit ScopedCollisionWarner(ThreadCollisionWarner* warner)
      : warner_(warner) {
    warner_->EnterSelf();
  }
  ~ScopedCollisionWarner() { warner_->LeaveSelf(); }

 private:
  ThreadCollisionWarner* warner_;
};

class ScopedRecursiveCollisionWarner {
 public:
  explicit ScopedRecursiveCollisionWarner(ThreadCollisionWarner* warner)
      : warner_(warner) {
    warner_->EnterSelfRecursive();
  }
  ~ScopedRecursiveCollisionWarner() { warner_->LeaveSelfRecursive(); }

 private:
  ThreadCollisionWarner* warner_;
};

#define DFAKE_MUTEX(name) ThreadCollisionWarner name
#define DFAKE_SCOPED_LOCK(name) ScopedCollisionWarner _scoped_##name(&name)
#define DFAKE_SCOPED_RECURSIVE_LOCK(name) \
  ScopedRecursiveCollisionWarner _scoped_recursive_##name(&name)

#else  // NDEBUG

#define DFAKE_MUTEX(name)
#define DFAKE_SCOPED_LOCK(name) ((void)0)
#define DFAKE_SCOPED_RECURSIVE_LOCK(name) ((void)0)

#endif
