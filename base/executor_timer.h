#pragma once

#include "base/executor.h"

#include <memory>

class ExecutorTimer {
 public:
  ExecutorTimer(std::shared_ptr<Executor> executor)
      : executor_{std::move(executor)} {}

  void StartOne(Duration period, std::function<void()> callback) {
    core_ = std::make_shared<Core>(executor_, period, std::move(callback));
    core_->Start<false>();
  }

  void StartRepeating(Duration period, std::function<void()> callback) {
    core_ = std::make_shared<Core>(executor_, period, std::move(callback));
    core_->Start<true>();
  }

  void Stop() { core_ = nullptr; }

 private:
  class Core : public std::enable_shared_from_this<Core> {
   public:
    Core(std::shared_ptr<Executor> executor,
         Duration period,
         std::function<void()> callback)
        : executor_{std::move(executor)},
          period_{period},
          callback_{std::move(callback)} {}

    template <bool kRepeating>
    void Start() {
      executor_->PostDelayedTask(period_, [weak_core = weak_from_this()] {
        if (auto core = weak_core.lock())
          core->callback_();
        if (kRepeating) {
          if (auto core = weak_core.lock())
            core->Start<kRepeating>();
        }
      });
    }

   private:
    const std::shared_ptr<Executor> executor_;
    const Duration period_;
    const std::function<void()> callback_;
  };

  const std::shared_ptr<Executor> executor_;

  std::shared_ptr<Core> core_;
};