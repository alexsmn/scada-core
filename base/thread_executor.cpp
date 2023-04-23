#include "base/thread_executor.h"

#include <cassert>

// ThreadExecutor::State

struct ThreadExecutor::State : public std::enable_shared_from_this<State> {
  struct PendingTask {
    bool operator<(const PendingTask& other) const;

    Task task;
    TimePoint time;
    int sequence = 0;
#ifndef NDEBUG
    boost::source_location location;
#endif
  };

  void Start();
  void Stop();

  Task GetTask();
  Task GetImmediateTask();

  void PostDelayedTask(Duration delay,
                       Task task,
                       const boost::source_location& location);
  size_t GetTaskCount() const;

  mutable std::mutex mutex_;
  std::condition_variable condition_;
  std::queue<Task> task_queue_;
  std::priority_queue<PendingTask> pending_task_queue_;
  int sequence_ = 0;
  std::jthread thread_;
  std::atomic<bool> stopped_ = false;
};

void ThreadExecutor::State::Start() {
  thread_ = std::jthread([this, ref = shared_from_this()] {
    while (!stopped_) {
      if (auto task = GetTask())
        task();
    }
    // Run all tasks on shutdown.
    while (auto task = GetImmediateTask())
      task();
  });
}

void ThreadExecutor::State::Stop() {
  if (stopped_) {
    return;
  }

  stopped_ = true;
  condition_.notify_all();

  // Thread can be shutdown from it's own task.
  if (std::this_thread::get_id() == thread_.get_id()) {
    // The `State` is still referenced by the thread. It will be released once
    // all pending tasks are executed.
    thread_.detach();
  } else {
    // Block until all pending tasks are executed.
    thread_.join();
  }
}

void ThreadExecutor::State::PostDelayedTask(
    Duration delay,
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

Executor::Task ThreadExecutor::State::GetTask() {
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

Executor::Task ThreadExecutor::State::GetImmediateTask() {
  std::unique_lock lock(mutex_);

  if (task_queue_.empty()) {
    return nullptr;
  }

  auto task = std::move(task_queue_.front());
  task_queue_.pop();
  return task;
}

size_t ThreadExecutor::State::GetTaskCount() const {
  std::unique_lock lock(mutex_);
  return pending_task_queue_.size();
}

// ThreadExecutor::State::PendingTask

bool ThreadExecutor::State::PendingTask::operator<(
    const PendingTask& other) const {
  if (time != other.time)
    return time < other.time;
  return sequence - other.sequence < 0;
}

// ThreadExecutor

ThreadExecutor::ThreadExecutor() : state_{std::make_shared<State>()} {
  state_->Start();
}

ThreadExecutor::~ThreadExecutor() {
  state_->Stop();
}

void ThreadExecutor::Shutdown() {
  state_->Stop();
}

void ThreadExecutor::PostDelayedTask(Duration delay,
                                     Task task,
                                     const boost::source_location& location) {
  state_->PostDelayedTask(delay, std::move(task), location);
}

size_t ThreadExecutor::GetTaskCount() const {
  return state_->GetTaskCount();
}
