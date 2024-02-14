#pragma once

#include "base/cancelation.h"
#include "base/common_types.h"

#include <functional>
#include <memory>
#include <source_location>
#include <stop_token>

class Executor {
 public:
  virtual ~Executor() {}

  using Task = std::function<void()>;

  void PostTask(
      Task task,
      const std::source_location& location = std::source_location::current()) {
    PostDelayedTask(Duration(), std::move(task), location);
  }

  virtual void PostDelayedTask(Duration delay,
                               Task task,
                               const std::source_location& location =
                                   std::source_location::current()) = 0;

  virtual size_t GetTaskCount() const = 0;
};

template <class T>
inline void Dispatch(
    Executor& executor,
    T&& task,
    const std::source_location& location = std::source_location::current()) {
  executor.PostTask(std::forward<T>(task), location);
}

namespace internal {

template <class F>
struct ExecutorBoundFunc {
  template <class U>
  ExecutorBoundFunc(std::shared_ptr<Executor> executor,
                    U&& func,
                    const std::source_location& location)
      : executor_{std::move(executor)},
        func_{std::forward<U>(func)}
#ifndef NDEBUG
        ,
        location_{location}
#endif
  {
  }

  template <class... Args>
  void operator()(Args&&... args) const {
    // https://stackoverflow.com/questions/47496358/c-lambdas-how-to-capture-variadic-parameter-pack-from-the-upper-scope
    auto closure = [func = func_, args = std::make_tuple(
                                      std::forward<Args>(args)...)]() mutable {
      std::apply(std::move(func), args);
    };

    Dispatch(*executor_, std::move(closure),
#ifndef NDEBUG
             location_
#else
             {}
#endif
    );
  }

  const std::shared_ptr<Executor> executor_;

  // WARNING: The operator may be called for multiple times. The functor must
  // not be moved.
  const F func_;

#ifndef NDEBUG
  const std::source_location location_;
#endif
};

}  // namespace internal

template <class T>
inline auto BindExecutor(
    std::shared_ptr<Executor> executor,
    T&& task,
    const std::source_location& location = std::source_location::current()) {
  return internal::ExecutorBoundFunc<T>{std::move(executor),
                                        std::forward<T>(task), location};
}

template <class C, class T>
inline auto BindExecutor(
    std::shared_ptr<Executor> executor,
    std::weak_ptr<C> cancelation,
    T&& task,
    const std::source_location& location = std::source_location::current()) {
  return BindExecutor(
      std::move(executor),
      BindCancelation(std::move(cancelation), std::forward<T>(task)), location);
}

template <class T>
inline auto BindExecutor(
    std::shared_ptr<Executor> executor,
    const Cancelation& cancelation,
    T&& task,
    const std::source_location& location = std::source_location::current()) {
  return BindExecutor(std::move(executor),
                      cancelation.Bind(std::forward<T>(task)), location);
}

template <class T>
inline auto BindExecutor(
    std::shared_ptr<Executor> executor,
    std::stop_token stop_token,
    T&& task,
    const std::source_location& location = std::source_location::current()) {
  return BindExecutor(
      std::move(executor),
      BindStopToken(std::move(stop_token), std::forward<T>(task)), location);
}

template <class C, class T>
inline void Dispatch(
    Executor& executor,
    std::weak_ptr<C> cancelation,
    T&& task,
    const std::source_location& location = std::source_location::current()) {
  Dispatch(executor,
           BindCancelation(std::move(cancelation), std::forward<T>(task)),
           location);
}

template <class T>
inline void Dispatch(
    Executor& executor,
    const Cancelation& cancelation,
    T&& task,
    const std::source_location& location = std::source_location::current()) {
  Dispatch(executor, cancelation.Bind(std::forward<T>(task)), location);
}
