#pragma once

#include "base/executor.h"

#include <boost/assert/source_location.hpp>
#include <memory>

class ExecutorTimer {
 public:
  ExecutorTimer(std::shared_ptr<Executor> executor)
      : executor_{std::move(executor)} {}

  void StartOne(
      Duration period,
      std::function<void()> callback,
      const boost::source_location& location = BOOST_CURRENT_LOCATION) {
    core_ = std::make_shared<Core>(executor_, period, std::move(callback),
                                   location);
    core_->Start<false>();
  }

  void StartRepeating(
      Duration period,
      std::function<void()> callback,
      const boost::source_location& location = BOOST_CURRENT_LOCATION) {
    core_ = std::make_shared<Core>(executor_, period, std::move(callback),
                                   location);
    core_->Start<true>();
  }

  void Stop() { core_ = nullptr; }

 private:
  class Core : public std::enable_shared_from_this<Core> {
   public:
    Core(std::shared_ptr<Executor> executor,
         Duration period,
         std::function<void()> callback,
         const boost::source_location& location)
        : executor_{std::move(executor)},
#ifndef NDEBUG
          location_{location},
#endif
          period_{period},
          callback_{std::move(callback)} {
    }

    template <bool kRepeating>
    void Start() {
      executor_->PostDelayedTask(
          period_,
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
    const std::shared_ptr<Executor> executor_;
    const Duration period_;
    const std::function<void()> callback_;
#ifndef NDEBUG
    const boost::source_location location_;
#endif
  };

  const std::shared_ptr<Executor> executor_;

  std::shared_ptr<Core> core_;
};

inline void StartRepeatableTimer(
    std::shared_ptr<Executor> executor,
    Duration period,
    const std::weak_ptr<bool>& cancelation,
    Executor::Task task,
    const boost::source_location& location = BOOST_CURRENT_LOCATION) {
  // |BindCancelation| is not effective, as |cancelation| has to be captured by
  // the lambda anyway.
  executor->PostDelayedTask(
      period,
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
