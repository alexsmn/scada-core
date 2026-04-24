#pragma once

#include "base/executor.h"

#include <boost/asio/execution.hpp>
#include <boost/asio/execution_context.hpp>
#include <algorithm>
#include <memory>
#include <mutex>
#include <source_location>
#include <vector>

// Adapts a shared_ptr<Executor> to satisfy the Boost.Asio executor concept,
// allowing it to be type-erased into AnyExecutor.
class ExecutorAdapter {
 public:
  explicit ExecutorAdapter(std::shared_ptr<Executor> executor)
      : state_{GetSharedState(std::move(executor))} {}

  bool operator==(const ExecutorAdapter&) const noexcept = default;

  boost::asio::execution_context& query(
      boost::asio::execution::context_t) const noexcept {
    return state_->context;
  }

  static constexpr boost::asio::execution::blocking_t::never_t query(
      boost::asio::execution::blocking_t) noexcept {
    return boost::asio::execution::blocking.never;
  }

  template <class F>
  void execute(F&& f,
               const std::source_location& location =
                   std::source_location::current()) const {
    state_->executor->PostTask(MakeExecutorTask(std::forward<F>(f)), location);
  }

 private:
  struct State {
    explicit State(std::shared_ptr<Executor> executor)
        : executor{std::move(executor)} {}

    boost::asio::execution_context context;
    std::shared_ptr<Executor> executor;
  };

  template <class F>
  static Executor::Task MakeExecutorTask(F f) {
    return [copyable_fun = std::make_shared<F>(std::move(f))]() mutable {
      std::move(*copyable_fun)();
    };
  }

  static std::shared_ptr<State> GetSharedState(
      std::shared_ptr<Executor> executor) {
    struct RegistryEntry {
      std::weak_ptr<Executor> executor;
      std::weak_ptr<State> state;
    };

    auto has_same_owner = [](const std::weak_ptr<Executor>& a,
                             const std::shared_ptr<Executor>& b) {
      return !a.owner_before(b) && !b.owner_before(a);
    };

    static std::mutex mutex;
    static std::vector<RegistryEntry> registry;

    std::lock_guard lock{mutex};
    std::erase_if(registry, [](const RegistryEntry& entry) {
      return entry.state.expired();
    });

    auto it = std::find_if(
        registry.begin(), registry.end(),
        [&](const RegistryEntry& entry) {
          return has_same_owner(entry.executor, executor);
        });
    if (it != registry.end()) {
      if (auto state = it->state.lock()) {
        return state;
      }
    }

    auto state = std::make_shared<State>(std::move(executor));
    registry.push_back(RegistryEntry{state->executor, state});
    return state;
  }

  std::shared_ptr<State> state_;
};

static_assert(boost::asio::execution::is_executor_v<ExecutorAdapter>);
