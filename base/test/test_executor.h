#pragma once

#include "base/executor.h"

#include <algorithm>

class TestExecutor : public Executor {
 public:
  explicit TestExecutor(bool instant = false) : instant_{instant} {}

  virtual void PostDelayedTask(Duration delay, Task task) override {
    if (instant_ || delay == Duration{})
      task();
    else
      pending_tasks_.emplace_back(delay, std::move(task));
  }

  virtual size_t GetTaskCount() const override { return 0; }

  void Advance(Duration delta) {
    auto p =
        std::stable_partition(pending_tasks_.begin(), pending_tasks_.end(),
                              [delta](auto& p) { return p.first < delta; });
    std::stable_sort(p, pending_tasks_.end(),
                     [](auto& a, auto& b) { return a.first < b.first; });
    std::vector<Task> run_tasks;
    run_tasks.reserve(pending_tasks_.end() - p);
    for (auto i = p; i != pending_tasks_.end(); ++i)
      run_tasks.emplace_back(std::move(i->second));
    pending_tasks_.erase(p, pending_tasks_.end());
    for (auto& t : pending_tasks_)
      t.first -= delta;
    for (auto& task : run_tasks)
      task();
  }

 private:
  const bool instant_;

  std::vector<std::pair<Duration, Task>> pending_tasks_;
};
