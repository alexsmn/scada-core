#pragma once

#include "base/executor.h"

#include <boost/asio/executor.hpp>
#include <memory>

class AsioExecutorAdapter {
 public:
  explicit AsioExecutorAdapter(std::shared_ptr<Executor> executor)
      : executor_{std::move(executor)} {}
  ~AsioExecutorAdapter() = default;

  AsioExecutorAdapter(const AsioExecutorAdapter& other)
      : executor_{other.executor_} {}

  AsioExecutorAdapter(AsioExecutorAdapter&& other) noexcept
      : executor_{std::move(other.executor_)} {}

  bool operator==(const AsioExecutorAdapter& other) const {
    return executor_ == other.executor_;
  }

  void on_work_started() {}
  void on_work_finished() {}

  boost::asio::execution_context& context() { return context_; }

  template <class F, class A>
  void dispatch(F&& f, A&& a) const {
    executor_->PostTask(
        MakeExecutorTask(std::forward<F>(f), std::forward<A>(a)));
  }

  template <class F, class A>
  void post(F&& f, A&& a) const {
    executor_->PostTask(
        MakeExecutorTask(std::forward<F>(f), std::forward<A>(a)));
  }

  template <class F, class A>
  void defer(F&& f, A&& a) const {
    executor_->PostTask(
        MakeExecutorTask(std::forward<F>(f), std::forward<A>(a)));
  }

 private:
  template <class F, class A>
  static Executor::Task MakeExecutorTask(F&& f, A&&) {
    return [copyable_fun = std::make_shared<F>(std::forward<F>(f))] {
      (*copyable_fun)();
    };
  }

  std::shared_ptr<Executor> executor_;

  boost::asio::execution_context context_;
};
