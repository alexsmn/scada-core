#pragma once

#include "base/executor.h"
#include "base/executor_util.h"

class AsioExecutor : public Executor {
 public:
  explicit AsioExecutor(const boost::asio::any_io_executor& executor)
      : executor_{executor} {}

  virtual void PostDelayedTask(Duration delay,
                               Task task,
                               const std::source_location& location =
                                   std::source_location::current()) override {
    ::PostDelayedTask(executor_, delay, std::move(task), location);
  }

  virtual size_t GetTaskCount() const override { return 0; }

 private:
  boost::asio::any_io_executor executor_;
};
