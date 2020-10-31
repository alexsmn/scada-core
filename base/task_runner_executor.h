#pragma once

#include "base/bind_util.h"
#include "base/executor.h"

class TaskRunnerExecutor : public Executor {
 public:
  explicit TaskRunnerExecutor(scoped_refptr<base::TaskRunner> task_runner)
      : task_runner_{std::move(task_runner)} {}

  virtual void PostDelayedTask(Duration delay, Task task) override {
    task_runner_->PostDelayedTask(
        FROM_HERE, BindLambda([task] { task(); }),
        base::TimeDelta::FromMicroseconds(
            std::chrono::duration_cast<std::chrono::microseconds>(delay)
                .count()));
  }

  virtual size_t GetTaskCount() const override { return 0; }

 private:
  const scoped_refptr<base::TaskRunner> task_runner_;
};