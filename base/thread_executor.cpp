#include "base/thread_executor.h"

#include <cassert>

bool ThreadExecutor::PendingTask::operator<(const PendingTask& other) const {
  if (time != other.time)
    return time < other.time;
  return sequence - other.sequence < 0;
}

ThreadExecutor::ThreadExecutor() {
  thread_ = std::thread([this] {
    while (!stopped_) {
      if (auto task = GetTask())
        task();
    }
    // Run all tasks on shutdown.
    while (auto task = GetImmediateTask())
      task();
  });
}

ThreadExecutor::~ThreadExecutor() {
  if (!stopped_)
    Shutdown();
}

void ThreadExecutor::Shutdown() {
  assert(!stopped_);
  stopped_ = true;
  condition_.notify_all();
  thread_.join();
}

void ThreadExecutor::PostDelayedTask(Duration delay,
                                     Task task,
                                     const boost::source_location& location) {
  std::lock_guard lock(mutex_);
  if (delay == Duration()) {
    task_queue_.emplace(std::move(task));
  } else {
    PendingTask pending_task = {
        .task = std::move(task),
        .time = Clock::now() + delay,
        .sequence = sequence_++,
#ifndef NDEBUG
        .location = location,
#endif
    };
    pending_task_queue_.emplace(std::move(pending_task));
  }
  condition_.notify_one();
}

Executor::Task ThreadExecutor::GetTask() {
  std::unique_lock lock(mutex_);

  while (!stopped_) {
    while (!pending_task_queue_.empty() &&
           Clock::now() >= pending_task_queue_.top().time) {
      task_queue_.emplace(pending_task_queue_.top().task);
      pending_task_queue_.pop();
    }

    if (!task_queue_.empty()) {
      auto task = std::move(task_queue_.front());
      task_queue_.pop();
      return task;
    }

    if (pending_task_queue_.empty()) {
      condition_.wait(lock);
    } else {
      auto delay = pending_task_queue_.top().time - Clock::now();
      condition_.wait_for(lock, delay);
    }
  }

  return nullptr;
}

Executor::Task ThreadExecutor::GetImmediateTask() {
  std::unique_lock lock(mutex_);

  if (task_queue_.empty()) {
    return nullptr;
  }

  auto task = std::move(task_queue_.front());
  task_queue_.pop();
  return task;
}

size_t ThreadExecutor::GetTaskCount() const {
  std::unique_lock lock(mutex_);
  return pending_task_queue_.size();
}
