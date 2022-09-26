#pragma once

#include "base/cancelation.h"
#include "base/common_types.h"

#include <boost/assert/source_location.hpp>
#include <functional>
#include <memory>

class Executor {
 public:
  virtual ~Executor() {}

  using Task = std::function<void()>;

  void PostTask(
      Task task,
      const boost::source_location& location = BOOST_CURRENT_LOCATION) {
    PostDelayedTask(Duration(), std::move(task), location);
  }

  virtual void PostDelayedTask(
      Duration delay,
      Task task,
      const boost::source_location& location = BOOST_CURRENT_LOCATION) = 0;

  virtual size_t GetTaskCount() const = 0;
};

template <class T>
inline void Dispatch(
    Executor& executor,
    T&& task,
    const boost::source_location& location = BOOST_CURRENT_LOCATION) {
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
        {}
#endif
    );
  }

  const std::shared_ptr<Executor> executor_;
  Task task_;
#ifndef NDEBUG
  const boost::source_location location_;
#endif
};

}  // namespace internal

template <class T>
inline auto BindExecutor(
    std::shared_ptr<Executor> executor,
    T&& task,
    const boost::source_location& location = BOOST_CURRENT_LOCATION) {
  return internal::ExecutorWrapper<T>{std::move(executor), std::forward<T>(task)
#ifndef NDEBUG
                                                               ,
                                      location
#endif
  };
}

template <class C, class T>
inline auto BindExecutor(
    std::shared_ptr<Executor> executor,
    std::weak_ptr<C> cancelation,
    T&& task,
    const boost::source_location& location = BOOST_CURRENT_LOCATION) {
  return BindExecutor(
      std::move(executor),
      BindCancelation(std::move(cancelation), std::forward<T>(task)), location);
}

template <class T>
inline auto BindExecutor(
    std::shared_ptr<Executor> executor,
    const Cancelation& cancelation,
    T&& task,
    const boost::source_location& location = BOOST_CURRENT_LOCATION) {
  return BindExecutor(std::move(executor),
                      cancelation.Bind(std::forward<T>(task)), location);
}

template <class C, class T>
inline void Dispatch(
    Executor& executor,
    std::weak_ptr<C> cancelation,
    T&& task,
    const boost::source_location& location = BOOST_CURRENT_LOCATION) {
  Dispatch(executor,
           BindCancelation(std::move(cancelation), std::forward<T>(task)),
           location);
}

template <class T>
inline void Dispatch(
    Executor& executor,
    const Cancelation& cancelation,
    T&& task,
    const boost::source_location& location = BOOST_CURRENT_LOCATION) {
  Dispatch(executor, cancelation.Bind(std::forward<T>(task)), location);
}
