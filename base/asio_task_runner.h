#pragma once

#include "base/executor_util.h"

#include <base/sequenced_task_runner.h>

class AsioTaskRunner : public base::SequencedTaskRunner {
 public:
  explicit AsioTaskRunner(const boost::asio::any_io_executor& executor)
      : executor_{executor} {}

  // base::SequencedTaskRunner
  virtual bool PostDelayedTask(const base::Location& from_here,
                               base::OnceClosure task,
                               base::TimeDelta delay) override;
  virtual bool RunsTasksInCurrentSequence() const override;
  virtual bool PostNonNestableDelayedTask(const base::Location& from_here,
                                          base::OnceClosure task,
                                          base::TimeDelta delay) override;

 private:
  boost::asio::any_io_executor executor_;
};

bool AsioTaskRunner::PostDelayedTask(const base::Location& from_here,
                                     base::OnceClosure task,
                                     base::TimeDelta delay) {
  ::PostDelayedTask(executor_, std::chrono::nanoseconds{delay.InNanoseconds()},
                    [task_ptr = std::make_shared<base::OnceClosure>(
                         std::move(task))] { std::move(*task_ptr).Run(); });
  return true;
}

bool AsioTaskRunner::RunsTasksInCurrentSequence() const {
  return true;
}

bool AsioTaskRunner::PostNonNestableDelayedTask(const base::Location& from_here,
                                                base::OnceClosure task,
                                                base::TimeDelta delay) {
  return PostDelayedTask(from_here, std::move(task), delay);
}
