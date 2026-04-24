#pragma once

#include "base/any_executor.h"
#include "base/awaitable.h"
#include "base/callback_awaitable.h"

#include <cassert>
#include <exception>
#include <functional>
#include <memory>
#include <vector>

namespace base {

// Executor-affine one-shot async completion gate.
//
// `AsyncCompletion` lets many coroutine waiters suspend until an owner calls
// `Complete()` or `Fail(error)`. Waiters that arrive after completion observe
// the stored result immediately, and failures are rethrown from `Wait()`.
//
// Example:
//   base::AsyncCompletion ready{executor};
//   CoSpawn(executor, [&]() -> Awaitable<void> {
//     co_await InitializeAsync();
//     ready.Complete();
//   });
//   co_await ready.Wait();
class AsyncCompletion {
 public:
  explicit AsyncCompletion(AnyExecutor executor) : executor_{std::move(executor)} {}

  [[nodiscard]] Awaitable<void> Wait() const {
    auto [error] = co_await CallbackToAwaitable<std::exception_ptr>(
        executor_, [this](auto callback) mutable {
          auto completion =
              std::make_shared<std::decay_t<decltype(callback)>>(
                  std::move(callback));

          if (completed_) {
            (*completion)(error_);
            return;
          }

          waiters_.emplace_back([completion](std::exception_ptr error) mutable {
            (*completion)(std::move(error));
          });
        });

    if (error) {
      std::rethrow_exception(error);
    }
    co_return;
  }

  void Complete() {
    Finish({});
  }

  void Fail(std::exception_ptr error) {
    assert(error);
    Finish(std::move(error));
  }

  [[nodiscard]] bool completed() const { return completed_; }

 private:
  using Handler = std::function<void(std::exception_ptr)>;

  void Finish(std::exception_ptr error) {
    assert(!completed_);
    if (completed_) {
      return;
    }

    completed_ = true;
    error_ = std::move(error);

    auto completion_error = error_;
    auto waiters = std::move(waiters_);
    for (auto& waiter : waiters) {
      waiter(completion_error);
    }
  }

  AnyExecutor executor_;
  bool completed_ = false;
  std::exception_ptr error_;
  mutable std::vector<Handler> waiters_;
};

}  // namespace base
