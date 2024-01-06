#pragma once

#include "base/cancelation.h"
#include "base/executor.h"
#include "base/promise.h"

#include <boost/assert/source_location.hpp>

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
  WrappedPromiseTask(const boost::source_location& location,
                     std::shared_ptr<Executor> executor,
                     T&& task)
      : executor_{std::move(executor)},
#ifndef NDEBUG
        location_{location},
#endif
        task_{std::forward<T>(task)} {
  }

  template <class... Args>
  auto operator()(Args&&... args) {
    auto promise = make_promise<std::invoke_result_t<Task, Args...>>();
    // https://stackoverflow.com/questions/47496358/c-lambdas-how-to-capture-variadic-parameter-pack-from-the-upper-scope
    Dispatch(
        *executor_,
        [promise, task = std::move(task_),
         args = std::make_tuple(std::forward<Args>(args)...)]() mutable {
          ResolvePromise(promise, std::move(task), std::move(args));
        },
#ifdef NDEBUG
        {}
#else
        location_
#endif
    );
    return promise;
  }

 private:
  const std::shared_ptr<Executor> executor_;
#ifndef NDEBUG
  const boost::source_location location_;
#endif
  Task task_;
};

template <class Task>
class WrappedPromiseTaskWithResult {
 public:
  template <class T>
  WrappedPromiseTaskWithResult(const boost::source_location& location,
                               std::shared_ptr<Executor> executor,
                               T&& task)
      : executor_{std::move(executor)},
#ifndef NDEBUG
        location_{location},
#endif
        task_{std::forward<T>(task)} {
  }

  template <class... Args>
  auto operator()(Args&&... args) {
    promise<promise_result_t<std::invoke_result_t<Task, Args...>>> promise;
    // https://stackoverflow.com/questions/47496358/c-lambdas-how-to-capture-variadic-parameter-pack-from-the-upper-scope
    Dispatch(
        *executor_,
        [promise, task = std::move(task_),
         args = std::make_tuple(std::forward<Args>(args)...)]() mutable {
          auto task_promise = std::apply(std::move(task), std::move(args));
          ForwardPromise(task_promise, promise);
        },
#ifdef NDEBUG
        {}
#else
        location_
#endif
    );
    return promise;
  }

 private:
  const std::shared_ptr<Executor> executor_;
#ifndef NDEBUG
  const boost::source_location location_;
#endif
  Task task_;
};

}  // namespace internal

template <class T>
inline auto BindPromiseExecutor(
    std::shared_ptr<Executor> executor,
    T&& task,
    const boost::source_location& location = BOOST_CURRENT_LOCATION) {
  return internal::WrappedPromiseTask<T>(location, std::move(executor),
                                         std::forward<T>(task));
}

template <class T, class C>
inline auto BindPromiseExecutor(
    std::shared_ptr<Executor> executor,
    std::weak_ptr<C> cancelation,
    T&& task,
    const boost::source_location& location = BOOST_CURRENT_LOCATION) {
  return BindPromiseExecutor(
      std::move(executor),
      BindCancelation(std::move(cancelation), std::forward<T>(task)), location);
}

template <class T>
inline auto BindPromiseExecutorWithResult(
    std::shared_ptr<Executor> executor,
    T&& task,
    const boost::source_location& location = BOOST_CURRENT_LOCATION) {
  return internal::WrappedPromiseTaskWithResult<T>(
      location, std::move(executor), std::forward<T>(task));
}

namespace internal {

template <class T>
struct CancelationPromiseFunc {
  template <class... Args>
  auto operator()(Args&&... args) const {
    using ReturnPromiseType = std::invoke_result_t<T, Args...>;
    return make_rejected_promise<typename ReturnPromiseType::value_type>(
        std::exception{});
  }
};

}  // namespace internal

template <class F>
inline auto DispatchPromise(Executor& executor, F&& func) {
  using P = std::invoke_result_t<F>;
  P p;

  Dispatch(executor, [func = std::forward<F>(func), p]() mutable {
    ForwardPromise(func(), p);
  });

  return p;
}

template <class T, class C>
inline auto BindPromiseExecutorWithResult(
    std::shared_ptr<Executor> executor,
    std::weak_ptr<C> cancelation,
    T&& task,
    const boost::source_location& location = BOOST_CURRENT_LOCATION) {
  return BindPromiseExecutorWithResult(
      std::move(executor),
      BindCancelationFunc(std::move(cancelation), std::forward<T>(task),
                          internal::CancelationPromiseFunc<T>{}),
      location);
}

inline promise<void> Delay(
    Executor& executor,
    Duration delay,
    const boost::source_location& location = BOOST_CURRENT_LOCATION) {
  promise<void> p;
  executor.PostDelayedTask(delay, std::bind_front(&promise<void>::resolve, p),
                           location);
  return p;
}