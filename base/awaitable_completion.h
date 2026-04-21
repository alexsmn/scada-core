#pragma once

#include "base/any_executor.h"
#include "base/awaitable.h"
#include "base/awaitable_promise.h"
#include "base/promise.h"

#include <cassert>
#include <exception>
#include <functional>
#include <memory>
#include <mutex>

class AwaitableCompletion {
 public:
  AwaitableCompletion() : state_{std::make_shared<State>()} {}

  static AwaitableCompletion Resolved() {
    AwaitableCompletion completion;
    completion.Complete();
    return completion;
  }

  [[nodiscard]] bool completed() const {
    std::lock_guard lock{state_->mutex};
    return state_->completed;
  }

  [[nodiscard]] std::exception_ptr error() const {
    std::lock_guard lock{state_->mutex};
    return state_->error;
  }

  void Complete() const {
    promise<void> completion;
    {
      std::lock_guard lock{state_->mutex};
      if (state_->completed) {
        return;
      }

      state_->completed = true;
      completion = state_->completion;
    }

    completion.resolve();
  }

  void Fail(std::exception_ptr error) const {
    assert(error);

    promise<void> completion;
    {
      std::lock_guard lock{state_->mutex};
      if (state_->completed) {
        return;
      }

      state_->completed = true;
      state_->error = error;
      completion = state_->completion;
    }

    completion.reject(error);
  }

  [[nodiscard]] Awaitable<void> Await(AnyExecutor executor) const {
    co_await AwaitPromise(std::move(executor), state_->completion);
  }

 private:
  struct State {
    mutable std::mutex mutex;
    bool completed = false;
    std::exception_ptr error;
    promise<void> completion;
  };

  std::shared_ptr<State> state_;
};
