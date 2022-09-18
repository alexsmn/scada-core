#pragma once

#include "base/cancelation.h"
#include "base/common_types.h"
#include "base/location.h"

#include <functional>
#include <memory>

class Executor {
 public:
  virtual ~Executor() {}

  using Task = std::function<void()>;

  void PostTask(Task task, const base::Location& location = FROM_HERE) {
    PostDelayedTask(Duration(), std::move(task), location);
  }

  virtual void PostDelayedTask(Duration delay,
                               Task task,
                               const base::Location& location = FROM_HERE) = 0;

  virtual size_t GetTaskCount() const = 0;
};

template <class T>
inline void Dispatch(Executor& executor,
                     T&& task,
                     const base::Location& location = FROM_HERE) {
  executor.PostTask(std::forward<T>(task), location);
}

namespace internal {

template <class Task>
struct ExecutorWrapper {
  template <class... Args>
  void operator()(Args&&... args) const {
    // https://stackoverflow.com/questions/47496358/c-lambdas-how-to-capture-variadic-parameter-pack-from-the-upper-scope
    Dispatch(
        *executor_,
        [task = std::move(task_),
         args = std::make_tuple(std::forward<Args>(args)...)]() mutable {
          std::apply(std::move(task), std::move(args));
        },
#ifdef NDEBUG
        location_
#else
        base::Location{}
#endif
    );
  }

  const std::shared_ptr<Executor> executor_;
  Task task_;
#ifndef NDEBUG
  const base::Location location_;
#endif
};

}  // namespace internal

template <class T>
inline auto BindExecutor(std::shared_ptr<Executor> executor,
                         T&& task,
                         const base::Location& location = FROM_HERE) {
  return internal::ExecutorWrapper<T>{std::move(executor), std::forward<T>(task)
#ifndef NDEBUG
                                                               ,
                                      location
#endif
  };
}

template <class C, class T>
inline auto BindExecutor(std::shared_ptr<Executor> executor,
                         std::weak_ptr<C> cancelation,
                         T&& task,
                         const base::Location& location = FROM_HERE) {
  return BindExecutor(
      std::move(executor),
      BindCancelation(std::move(cancelation), std::forward<T>(task)), location);
}

template <class T, class C>
inline void Dispatch(Executor& executor,
                     std::weak_ptr<C> cancelation,
                     T&& task,
                     const base::Location& location = FROM_HERE) {
  Dispatch(executor,
           BindCancelation(std::move(cancelation), std::forward<T>(task)),
           location);
}
