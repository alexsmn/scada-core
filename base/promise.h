#pragma once

#include "base/executor.h"

#include <promise.hpp/promise.hpp>

using namespace promise_hpp;

namespace internal {

template <class Task>
class WrappedPromiseTask {
 public:
  template <class T>
  WrappedPromiseTask(std::shared_ptr<Executor> executor, T&& task)
      : executor_{std::move(executor)}, task_{std::forward<T>(task)} {}

  template <class... Args>
  auto operator()(Args&&... args) const {
    auto promise = make_promise<std::invoke_result_t<Task, Args...>>();
    // https://stackoverflow.com/questions/47496358/c-lambdas-how-to-capture-variadic-parameter-pack-from-the-upper-scope
    Dispatch(*executor_,
             [promise, task = std::move(task_),
              args = std::make_tuple(std::forward<Args>(args)...)]() mutable {
               promise.resolve(std::apply(std::move(task), std::move(args)));
             });
    return promise;
  }

 private:
  const std::shared_ptr<Executor> executor_;
  Task task_;
};

}  // namespace internal

template <class T>
inline auto BindPromiseExecutor(std::shared_ptr<Executor> executor, T&& task) {
  return internal::WrappedPromiseTask<T>(std::move(executor),
                                         std::forward<T>(task));
}
