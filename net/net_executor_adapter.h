#pragma once

#include "base/executor.h"

#include <memory>
#include <transport/executor.h>
#include <source_location>

class NetExecutorAdapter {
 public:
  explicit NetExecutorAdapter(std::shared_ptr<Executor> executor)
      : context_{std::make_shared<boost::asio::execution_context>()},
        executor_{std::move(executor)} {}

  bool operator==(const NetExecutorAdapter&) const noexcept = default;

  boost::asio::execution_context& query(
      boost::asio::execution::context_t) const noexcept {
    return *context_;
  }

  static constexpr boost::asio::execution::blocking_t::never_t query(
      boost::asio::execution::blocking_t) noexcept {
    // This executor always has blocking.never semantics.
    return boost::asio::execution::blocking.never;
  }

  template <class F>
  void execute(F&& f,
               const std::source_location& location =
                   std::source_location::current()) const {
    executor_->PostTask(MakeExecutorTask(std::forward<F>(f)), location);
  }

 private:
  template <class F>
  static Executor::Task MakeExecutorTask(F f) {
    return [copyable_fun = std::make_shared<F>(std::move(f))]() mutable {
      std::move (*copyable_fun)();
    };
  }

  std::shared_ptr<boost::asio::execution_context> context_;
  std::shared_ptr<Executor> executor_;
};

static_assert(boost::asio::execution::is_executor_v<NetExecutorAdapter>);