#pragma once

#include "base/any_executor.h"
#include "base/common_types.h"
#include "base/debug_holder.h"

#include <boost/asio/steady_timer.hpp>
#include <functional>
#include <memory>
#include <source_location>

class AnyExecutorTimer {
 public:
  AnyExecutorTimer() = default;

  AnyExecutorTimer(AnyExecutor executor) : executor_{std::move(executor)} {}

  AnyExecutorTimer(AnyExecutorTimer&&) = default;
  AnyExecutorTimer& operator=(AnyExecutorTimer&&) = default;

  AnyExecutorTimer(const AnyExecutorTimer&) = delete;
  AnyExecutorTimer& operator=(const AnyExecutorTimer&) = delete;

  void StartOne(
      Clock::duration period,
      std::function<void()> callback,
      const std::source_location& location = std::source_location::current()) {
    core_ = std::make_shared<Core>(executor_, period, std::move(callback),
                                   location);
    core_->Start<false>();
  }

  void StartRepeating(
      Clock::duration period,
      std::function<void()> callback,
      const std::source_location& location = std::source_location::current()) {
    core_ = std::make_shared<Core>(executor_, period, std::move(callback),
                                   location);
    core_->Start<true>();
  }

  void Stop() { core_ = nullptr; }

 private:
  class Core : public std::enable_shared_from_this<Core> {
   public:
    Core(AnyExecutor executor,
         Clock::duration period,
         std::function<void()> callback,
         const std::source_location& location)
        : executor_{std::move(executor)},
#ifndef NDEBUG
          location_{location},
#endif
          period_{period},
          callback_{std::move(callback)} {
    }

    // Cancels the armed wait so the timer stops holding outstanding work on
    // the executor's io_context. Without this, dropping the Core (via Stop()
    // or destruction) leaves the pending wait behind: it keeps the context
    // "busy" with nothing runnable until the full period elapses, which
    // stalls ServerProcess's shutdown drain.
    ~Core() {
      if (timer_)
        timer_->cancel();
    }

    template <bool kRepeating>
    void Start() {
      timer_ = std::make_shared<boost::asio::steady_timer>(executor_);
      timer_->expires_after(period_);
      timer_->async_wait([weak_core = weak_from_this(),
                          timer = timer_](boost::system::error_code ec) {
        if (ec == boost::asio::error::operation_aborted)
          return;
        if (auto core = weak_core.lock())
          core->callback_();
        if constexpr (kRepeating) {
          if (auto core = weak_core.lock())
            core->Start<kRepeating>();
        }
      });
    }

   private:
    const AnyExecutor executor_;
    const Clock::duration period_;
    const std::function<void()> callback_;
#ifndef NDEBUG
    const std::source_location location_;
#endif
    // Kept so the Core can cancel an armed wait when it goes away.
    std::shared_ptr<boost::asio::steady_timer> timer_;
  };

  AnyExecutor executor_;

  std::shared_ptr<Core> core_;
};

inline void StartRepeatableTimer(
    AnyExecutor executor,
    Clock::duration period,
    const std::weak_ptr<bool>& cancelation,
    std::function<void()> task,
    const std::source_location& location = std::source_location::current()) {
  PostDelayedTask(
      executor, period,
      [executor, period, cancelation, task = std::move(task),
       location]() mutable {
        if (cancelation.expired()) {
          return;
        }
        task();
        StartRepeatableTimer(std::move(executor), period, cancelation,
                             std::move(task), location);
      },
      location);
}
