#pragma once

#include "base/any_executor.h"
#include "base/common_types.h"

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <source_location>
#include <thread>

class ThreadExecutor {
 public:
  using Task = std::function<void()>;

  ThreadExecutor();
  ~ThreadExecutor();

  ThreadExecutor(const ThreadExecutor&) = default;
  ThreadExecutor& operator=(const ThreadExecutor&) = default;

  bool operator==(const ThreadExecutor& other) const noexcept {
    return state_ == other.state_;
  }

  bool operator!=(const ThreadExecutor& other) const noexcept {
    return !(*this == other);
  }

  boost::asio::execution_context& query(
      boost::asio::execution::context_t) const noexcept;

  boost::asio::execution_context& context() const noexcept;

  static constexpr boost::asio::execution::blocking_t::never_t query(
      boost::asio::execution::blocking_t) noexcept {
    return boost::asio::execution::blocking.never;
  }

  template <class F>
  void execute(F&& f) const {
    PostTask(MakeTask(std::forward<F>(f)));
  }

  void on_work_started() const noexcept {}
  void on_work_finished() const noexcept {}

  template <class F, class Allocator>
  void dispatch(F&& f, const Allocator&) const {
    execute(std::forward<F>(f));
  }

  template <class F, class Allocator>
  void post(F&& f, const Allocator&) const {
    execute(std::forward<F>(f));
  }

  template <class F, class Allocator>
  void defer(F&& f, const Allocator&) const {
    execute(std::forward<F>(f));
  }

  // Blocks until all pending tasks are executed and stops the executor. Except
  // when called from an own task - in that case no blocking happens.
  void Shutdown();

  void PostTask(Task task,
                const std::source_location& location =
                    std::source_location::current()) const;
  void PostDelayedTask(Duration delay,
                       Task task,
                       const std::source_location& location =
                           std::source_location::current()) const;
  size_t GetTaskCount() const;

 private:
  struct State;

  template <class F>
  static Task MakeTask(F&& f) {
    using Func = std::decay_t<F>;
    return [copyable_fun =
                std::make_shared<Func>(std::forward<F>(f))]() mutable {
      std::move(*copyable_fun)();
    };
  }

  std::shared_ptr<State> state_;
};
