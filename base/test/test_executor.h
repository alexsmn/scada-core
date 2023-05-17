#pragma once

#include "base/executor.h"

#include <algorithm>

class TestExecutor : public Executor {
 public:
  explicit TestExecutor(bool instant = false) : instant_{instant} {}

  /*~TestExecutor() {
    // It's important to run all the remaining pending tasks. E.g.
    // HistoricalDatabase will only close on the posted task.
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

  virtual void PostDelayedTask(Duration delay,
                               Task task,
                               const boost::source_location& location =
                                   BOOST_CURRENT_LOCATION) override {
    if (instant_ || delay == Duration{}) {
      task();
    } else {
      pending_tasks_.emplace_back(delay, std::move(task), location);
    }
  }

  virtual size_t GetTaskCount() const override { return 0; }

  void Poll() { Advance({}); }

  void Advance(Duration delta) {
    auto run_tasks = PopRunTasks(delta);
    for (auto& t : pending_tasks_)
      t.delay -= delta;
    for (auto& task : run_tasks)
      task();
  }

 private:
  std::vector<Task> PopRunTasks(Duration delta) {
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
    return run_tasks;
  }

  const bool instant_;

  struct PendingTask {
    Duration delay;
    Task task;
    boost::source_location location;
  };

  std::vector<PendingTask> pending_tasks_;
};
