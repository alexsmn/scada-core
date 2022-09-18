#pragma once

#include "base/cancelation.h"
#include "base/executor.h"

#include <promise.hpp/promise.hpp>

using namespace promise_hpp;

namespace internal {

template <class T, class Args, class R>
inline void ResolvePromise(promise<R>& promise, T&& task, Args&& args) {
  promise.resolve(std::apply(std::forward<T>(task), std::forward<Args>(args)));
}

template <class T, class Args>
inline void ResolvePromise(promise<>& promise, T&& task, Args&& args) {
  std::apply(std::forward<T>(task), std::forward<Args>(args));
  promise.resolve();
}

template <class Task>
class WrappedPromiseTask {
 public:
  template <class T>
  WrappedPromiseTask(const base::Location& location,
                     std::shared_ptr<Executor> executor,
                     T&& task)
      : executor_{std::move(executor)},
#ifndef NDEBUG
        location_{location},
#endif
        task_{std::forward<T>(task)} {
  }

  template <class... Args>
  auto operator()(Args&&... args) const {
    auto promise = make_promise<std::invoke_result_t<Task, Args...>>();
    // https://stackoverflow.com/questions/47496358/c-lambdas-how-to-capture-variadic-parameter-pack-from-the-upper-scope
    Dispatch(
        *executor_,
        [promise, task = std::move(task_),
         args = std::make_tuple(std::forward<Args>(args)...)]() mutable {
          ResolvePromise(promise, std::move(task), std::move(args));
        },
#ifdef NDEBUG
        base::Location {}
#else
        location_
#endif
    );
    return promise;
  }

 private:
  const std::shared_ptr<Executor> executor_;
#ifndef NDEBUG
  const base::Location location_;
#endif
  Task task_;
};

}  // namespace internal

template <class T>
inline auto BindPromiseExecutor(std::shared_ptr<Executor> executor,
                                T&& task,
                                const base::Location& location = FROM_HERE) {
  return internal::WrappedPromiseTask<T>(location, std::move(executor),
                                         std::forward<T>(task));
}

template <class T, class C>
inline auto BindPromiseExecutor(std::shared_ptr<Executor> executor,
                                std::weak_ptr<C> cancelation,
                                T&& task,
                                const base::Location& location = FROM_HERE) {
  return BindPromiseExecutor(
      std::move(executor),
      BindCancelation(std::move(cancelation), std::forward<T>(task)), location);
}
