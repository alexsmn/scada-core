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
struct ExecutorWrapper {
  template <class... Args>
  void operator()(Args&&... args) const {
    // https://stackoverflow.com/questions/47496358/c-lambdas-how-to-capture-variadic-parameter-pack-from-the-upper-scope
    Dispatch(*executor_,
             [task = std::move(task_),
              args = std::make_tuple(std::forward<Args>(args)...)]() mutable {
               std::apply(std::move(task), std::move(args));
             });
  }

  const std::shared_ptr<Executor> executor_;
  Task task_;
};

template <class C, class T>
struct CancelationWrapper {
  template <class... Args>
  void operator()(Args&&... args) const {
    auto ref = cancelation_.lock();
    if (ref)
      std::move(task_)(std::forward<Args>(args)...);
  }

  const std::weak_ptr<C> cancelation_;
  T task_;
};

}  // namespace internal

template <class C, class T>
inline auto BindCancelation(std::weak_ptr<C> cancelation, T&& task) {
  return internal::CancelationWrapper<C, T>{std::move(cancelation),
                                            std::forward<T>(task)};
}

template <class T>
inline auto BindExecutor(std::shared_ptr<Executor> executor, T&& task) {
  return internal::ExecutorWrapper<T>{std::move(executor),
                                      std::forward<T>(task)};
}

template <class C, class T>
inline auto BindExecutor(std::shared_ptr<Executor> executor,
                         std::weak_ptr<C> cancelation,
                         T&& task) {
  return BindExecutor(
      std::move(executor),
      BindCancelation(std::move(cancelation), std::forward<T>(task)));
}
