#pragma once

#include "base/executor.h"

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <thread>

class ThreadExecutor : public Executor {
 public:
  ThreadExecutor();
  ~ThreadExecutor();

  ThreadExecutor(const ThreadExecutor&) = delete;
  ThreadExecutor& operator=(const ThreadExecutor&) = delete;

  // Blocks until all pending tasks are executed and stops the executor. Except
  // when called from an own task - in that case no blocking happens.
  void Shutdown();

  // Executor
  virtual void PostDelayedTask(
      Duration delay,
      Task task,
      const boost::source_location& location = BOOST_CURRENT_LOCATION) override;
  virtual size_t GetTaskCount() const override;

 private:
  struct State;

  std::shared_ptr<State> state_;
};
