#pragma once

#include "base/executor.h"

#include <algorithm>
#include <mutex>
#include <memory>
#include <transport/executor.h>
#include <source_location>
#include <vector>

class NetExecutorAdapter {
 public:
  explicit NetExecutorAdapter(std::shared_ptr<Executor> executor)
      : state_{GetSharedState(std::move(executor))} {}

  bool operator==(const NetExecutorAdapter&) const noexcept = default;

  boost::asio::execution_context& query(
      boost::asio::execution::context_t) const noexcept {
    return state_->context;
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
      std::move (*copyable_fun)();
    };
  }

  static std::shared_ptr<State> GetSharedState(
      std::shared_ptr<Executor> executor) {
    struct RegistryEntry {
      const Executor* executor = nullptr;
      std::weak_ptr<State> state;
    };

    static std::mutex mutex;
    static std::vector<RegistryEntry> registry;

    std::lock_guard lock{mutex};
    std::erase_if(registry, [](const RegistryEntry& entry) {
      return entry.state.expired();
    });

    const auto* executor_ptr = executor.get();
    auto it = std::find_if(
        registry.begin(), registry.end(),
        [executor_ptr](const RegistryEntry& entry) {
          return entry.executor == executor_ptr;
        });
    if (it != registry.end()) {
      if (auto state = it->state.lock()) {
        return state;
      }
    }

    auto state = std::make_shared<State>(std::move(executor));
    registry.push_back(RegistryEntry{executor_ptr, state});
    return state;
  }

  std::shared_ptr<State> state_;
};

static_assert(boost::asio::execution::is_executor_v<NetExecutorAdapter>);
