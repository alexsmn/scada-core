#pragma once

#include "base/common_types.h"

#include <functional>
#include <memory>

class Executor {
 public:
  virtual ~Executor() {}

  using Task = std::function<void()>;

  void PostTask(Task task) { PostDelayedTask(Duration(), std::move(task)); }

  virtual void PostDelayedTask(Duration delay, Task task) = 0;

  virtual size_t GetTaskCount() const = 0;
};

template <class T>
inline void Dispatch(Executor& executor, T&& task) {
  executor.PostTask(std::forward<T>(task));
}

namespace internal {

template <class Task>
class WrappedTask {
 public:
  template <class T>
  WrappedTask(std::shared_ptr<Executor> executor, T&& task)
      : executor_{std::move(executor)}, task_{std::forward<T>(task)} {}

  template <class... Args>
  void operator()(Args&&... args) const {
    // https://stackoverflow.com/questions/47496358/c-lambdas-how-to-capture-variadic-parameter-pack-from-the-upper-scope
    Dispatch(*executor_,
             [task = std::move(task_),
              args = std::make_tuple(std::forward<Args>(args)...)]() mutable {
               std::apply(std::move(task), std::move(args));
             });
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
