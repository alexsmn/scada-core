#pragma once

#include "base/common_types.h"

#include <memory>

class Executor {
 public:
  virtual ~Executor() {}

  void PostTask(Task task) { PostDelayedTask(Duration(), std::move(task)); }

  virtual void PostDelayedTask(Duration delay, Task task) = 0;

  virtual size_t GetTaskCount() const = 0;
};

inline void Dispatch(Executor& executor, Task task) {
  executor.PostTask(std::move(task));
}

namespace internal {

template <class Task>
class WrappedTask {
 public:
  template <class T>
  WrappedTask(std::shared_ptr<Executor> executor, T&& task)
      : executor_{std::move(executor)}, task_{std::forward<T>(task)} {}

  template <class... Args>
  void operator()(const Args&... args) const {
    Dispatch(*executor_, [task = std::move(task_), args...] { task(args...); });
  }

 private:
  const std::shared_ptr<Executor> executor_;
  Task task_;
};

}  // namespace internal

template <class T>
inline auto BindExecutor(std::shared_ptr<Executor> executor, T&& task) {
  return internal::WrappedTask<T>(std::move(executor), std::forward<T>(task));
}
