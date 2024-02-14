#pragma once

#include "base/cancelation.h"
#include "base/executor.h"
#include "base/promise.h"

#include <source_location>

// A `Dispatch` version that returns a promise.
// `closure` can a promise, or a value, or void.
template <class F>
inline [[nodiscard]] auto DispatchAsPromise(
    Executor& executor,
    F&& closure,
    const std::source_location& location = std::source_location::current()) {
  using R = std::invoke_result_t<F>;
  add_promise_t<R> p;

  Dispatch(
      executor,
      [closure = std::forward<F>(closure), p]() mutable {
        if constexpr (std::is_void_v<R>) {
          closure();
          p.resolve();
        } else {
          ForwardPromise(closure(), p);
        }
      },
      location);

  return p;
}

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

template <class F>
class WrappedPromiseTask {
 public:
  template <class U>
  WrappedPromiseTask(const std::source_location& location,
                     std::shared_ptr<Executor> executor,
                     U&& func)
      : executor_{std::move(executor)},
        func_{std::forward<U>(func)},
        location_{location} {}

  template <class... Args>
  auto operator()(Args&&... args) {
    using TaskResult = std::invoke_result_t<F, Args...>;
    auto promise = make_promise<TaskResult>();
    // https://stackoverflow.com/questions/47496358/c-lambdas-how-to-capture-variadic-parameter-pack-from-the-upper-scope
    Dispatch(
        *executor_,
        [promise, func = std::move(func_),
         args = std::make_tuple(std::forward<Args>(args)...)]() mutable {
          ResolvePromise(promise, std::move(func), std::move(args));
        },
        location_.get());
    return promise;
  }

 private:
  const std::shared_ptr<Executor> executor_;
  F func_;
  const DebugHolder<std::source_location> location_;
};

template <class F>
class WrappedPromiseTaskWithResult {
 public:
  template <class U>
  WrappedPromiseTaskWithResult(const std::source_location& location,
                               std::shared_ptr<Executor> executor,
                               U&& func)
      : executor_{std::move(executor)},
        func_{std::forward<U>(func)},
        location_{location} {}

  template <class... Args>
  auto operator()(Args&&... args) {
    auto closure =
        [func = std::move(func_),
         args = std::make_tuple(std::forward<Args>(args)...)]() mutable {
          return std::apply(std::move(func), std::move(args));
        };

    return DispatchAsPromise(*executor_, std::move(closure), location_.get());
  }

 private:
  const std::shared_ptr<Executor> executor_;
  F func_;
  const DebugHolder<std::source_location> location_;
};

}  // namespace internal

template <class T>
inline auto BindPromiseExecutor(
    std::shared_ptr<Executor> executor,
    T&& task,
    const std::source_location& location = std::source_location::current()) {
  return internal::WrappedPromiseTask<T>(location, std::move(executor),
                                         std::forward<T>(task));
}

template <class T, class C>
inline auto BindPromiseExecutor(
    std::shared_ptr<Executor> executor,
    std::weak_ptr<C> cancelation,
    T&& task,
    const std::source_location& location = std::source_location::current()) {
  return BindPromiseExecutor(
      std::move(executor),
      BindCancelation(std::move(cancelation), std::forward<T>(task)), location);
}

template <class T>
inline auto BindPromiseExecutorWithResult(
    std::shared_ptr<Executor> executor,
    T&& task,
    const std::source_location& location = std::source_location::current()) {
  return internal::WrappedPromiseTaskWithResult<T>(
      location, std::move(executor), std::forward<T>(task));
}

namespace internal {

template <class T>
struct CancelationPromiseFunc {
  template <class... Args>
  auto operator()(Args&&...) const {
    using FuncResultType = std::invoke_result_t<T, Args...>;
    return make_rejected_promise<remove_promise_t<FuncResultType>>(
        std::exception{});
  }
};

}  // namespace internal

template <class T, class C>
inline auto BindPromiseExecutorWithResult(
    std::shared_ptr<Executor> executor,
    std::weak_ptr<C> cancelation,
    T&& task,
    const std::source_location& location = std::source_location::current()) {
  return BindPromiseExecutorWithResult(
      std::move(executor),
      BindCancelationFunc(std::move(cancelation), std::forward<T>(task),
                          internal::CancelationPromiseFunc<T>{}),
      location);
}

template <class T, class C>
inline auto BindPromiseExecutorWithResult(
    std::shared_ptr<Executor> executor,
    const Cancelation& cancelation,
    T&& task,
    const std::source_location& location = std::source_location::current()) {
  return BindPromiseExecutorWithResult(
      std::move(executor),
      BindCancelationFunc(cancelation.weak_ptr(), std::forward<T>(task),
                          internal::CancelationPromiseFunc<T>{}),
      location);
}

inline promise<void> Delay(
    Executor& executor,
    Duration delay,
    const std::source_location& location = std::source_location::current()) {
  promise<void> p;
  executor.PostDelayedTask(delay, std::bind_front(&promise<void>::resolve, p),
                           location);
  return p;
}