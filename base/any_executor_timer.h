#pragma once

#include "base/any_executor.h"
#include "base/common_types.h"
#include "base/debug_holder.h"
#include "base/executor_util.h"

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
      Duration period,
      std::function<void()> callback,
      const std::source_location& location = std::source_location::current()) {
    core_ = std::make_shared<Core>(executor_, period, std::move(callback),
                                   location);
    core_->Start<false>();
  }

  void StartRepeating(
      Duration period,
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
         Duration period,
         std::function<void()> callback,
         const std::source_location& location)
        : executor_{std::move(executor)},
#ifndef NDEBUG
          location_{location},
#endif
          period_{period},
          callback_{std::move(callback)} {
    }

    template <bool kRepeating>
    void Start() {
      PostDelayedTask(
          executor_, period_,
          [weak_core = weak_from_this()] {
            if (auto core = weak_core.lock())
              core->callback_();
            if (kRepeating) {
              if (auto core = weak_core.lock())
                core->Start<kRepeating>();
            }
          },
#ifdef NDEBUG
          {}
#else
          location_
#endif
      );
    }

   private:
    const AnyExecutor executor_;
    const Duration period_;
    const std::function<void()> callback_;
#ifndef NDEBUG
    const std::source_location location_;
#endif
  };

  AnyExecutor executor_;

  std::shared_ptr<Core> core_;
};

inline void StartRepeatableTimer(
    AnyExecutor executor,
    Duration period,
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
