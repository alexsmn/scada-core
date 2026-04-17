#pragma once

#include "base/auto_reset.h"
#include "base/executor.h"
#include "base/executor_factory.h"

#include <algorithm>
#include <mutex>
#include <thread>
#include <vector>

class TestExecutor : public Executor {
 public:
  explicit TestExecutor(bool instant = false) : instant_{instant} {}

  /*~TestExecutor() {
    // It's important to run all the remaining pending tasks. E.g.
    // HistoricalDb will only close on the posted task.
    for (;;) {
      auto run_tasks = PopRunTasks(Duration());
      if (run_tasks.empty()) {
        break;
      }
      for (auto& task : run_tasks) {
        task();
      }
    }
  }*/

  bool is_current_executor() {
    return std::ranges::find(current_executor_stack_, this) !=
           current_executor_stack_.end();
  }

  virtual void PostDelayedTask(Duration delay,
                               Task task,
                               const std::source_location& location =
                                   std::source_location::current()) override {
    if (instant_) {
      ScopedCurrentExecutor current{this};
      task();
    } else {
      // Queue zero-delay work as well. Running it inline makes this executor
      // reentrant across foreign threads, which breaks Asio/coroutine adapter
      // paths that expect posted continuations to run later when polled.
      std::lock_guard lock{mutex_};
      pending_tasks_.emplace_back(delay, std::move(task), location);
    }
  }

  virtual size_t GetTaskCount() const override {
    std::lock_guard lock{mutex_};
    return pending_tasks_.size();
  }

  void Poll() { Advance({}); }

  void Advance(Duration delta) {
    ScopedCurrentExecutor current{this};

    auto run_tasks = PopRunTasks(delta);

    for (auto& task : run_tasks) {
      task();
    }
  }

 private:
  class ScopedCurrentExecutor {
   public:
    explicit ScopedCurrentExecutor(const TestExecutor* executor)
        : executor_{executor} {
      current_executor_stack_.push_back(executor_);
    }

    ~ScopedCurrentExecutor() { current_executor_stack_.pop_back(); }

   private:
    const TestExecutor* executor_;
  };

  std::vector<Task> PopRunTasks(Duration delta) {
    std::lock_guard lock{mutex_};

    // Move run tasks with |task.delay <= delta| to the end of queue.
    auto p = std::stable_partition(
        pending_tasks_.begin(), pending_tasks_.end(),
        [delta](const PendingTask& p) { return p.delay > delta; });

    // Sort run tasks.
    std::stable_sort(p, pending_tasks_.end(),
                     [](const PendingTask& a, const PendingTask& b) {
                       return a.delay < b.delay;
                     });

    // Collect run tasks
    std::vector<Task> run_tasks;
    run_tasks.reserve(pending_tasks_.end() - p);
    for (auto i = p; i != pending_tasks_.end(); ++i)
      run_tasks.emplace_back(std::move(i->task));

    // Remove run tasks.
    pending_tasks_.erase(p, pending_tasks_.end());

    for (auto& t : pending_tasks_) {
      t.delay -= delta;
    }

    return run_tasks;
  }

  const bool instant_;

  struct PendingTask {
    Duration delay;
    Task task;
    std::source_location location;
  };

  mutable std::mutex mutex_;
  std::vector<PendingTask> pending_tasks_;
  inline static thread_local std::vector<const TestExecutor*>
      current_executor_stack_;
};

inline ExecutorFactory MakeTestExecutorFactory() {
  return MakeSingleExecutorFactory(std::make_shared<TestExecutor>());
}

#include "base/any_executor.h"
#include "base/executor_adapter.h"

inline AnyExecutor MakeTestAnyExecutor(
    std::shared_ptr<TestExecutor> executor) {
  return AnyExecutor{ExecutorAdapter{std::move(executor)}};
}

inline AnyExecutorFactory MakeTestAnyExecutorFactory() {
  auto executor = std::make_shared<TestExecutor>();
  return [executor] { return MakeTestAnyExecutor(executor); };
}
