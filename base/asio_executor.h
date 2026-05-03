#pragma once

#include "base/any_executor.h"
#include "base/common_types.h"

#include <functional>
#include <source_location>

class AsioExecutor {
 public:
  using Task = std::function<void()>;

  explicit AsioExecutor(AnyExecutor executor) : executor_{std::move(executor)} {}

  void PostDelayedTask(Duration delay,
                       Task task,
                       const std::source_location& location =
                           std::source_location::current()) {
    ::PostDelayedTask(executor_, delay, std::move(task), location);
  }

  size_t GetTaskCount() const { return 0; }

 private:
  AnyExecutor executor_;
};
