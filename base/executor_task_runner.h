#pragma once

#include "base/executor.h"
#include "base/time_utils.h"

#include <base/sequenced_task_runner.h>
#include <memory>

class ExecutorTaskRunner : public base::SequencedTaskRunner {
 public:
  explicit ExecutorTaskRunner(std::shared_ptr<Executor> executor);

  // base::SequencedTaskRunner
  virtual bool PostDelayedTask(const base::Location& from_here,
                               base::OnceClosure task,
                               base::TimeDelta delay) override;
  virtual bool RunsTasksInCurrentSequence() const override;
  virtual bool PostNonNestableDelayedTask(const base::Location& from_here,
                                          base::OnceClosure task,
                                          base::TimeDelta delay) override;

 private:
  const std::shared_ptr<Executor> executor_;
};

inline ExecutorTaskRunner::ExecutorTaskRunner(
    std::shared_ptr<Executor> executor)
    : executor_{std::move(executor)} {}

inline bool ExecutorTaskRunner::PostDelayedTask(const base::Location& from_here,
                                                base::OnceClosure task,
                                                base::TimeDelta delay) {
  executor_->PostDelayedTask(
      AsChrono(delay), [task = std::make_shared<base::OnceClosure>(
                            std::move(task))] { std::move(*task).Run(); });

  return true;
}

inline bool ExecutorTaskRunner::RunsTasksInCurrentSequence() const {
  return true;
}

inline bool ExecutorTaskRunner::PostNonNestableDelayedTask(
    const base::Location& from_here,
    base::OnceClosure task,
    base::TimeDelta delay) {
  return PostDelayedTask(from_here, std::move(task), delay);
}

inline scoped_refptr<ExecutorTaskRunner> CreateExecutorTaskRunner(
    std::shared_ptr<Executor> executor) {
  return base::MakeRefCounted<ExecutorTaskRunner>(executor);
}
