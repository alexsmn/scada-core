#pragma once

#include "base/executor.h"
#include "base/io_context_util.h"

#include <boost/asio/io_context.hpp>

class AsioExecutor : public Executor {
 public:
  explicit AsioExecutor(boost::asio::io_context& io_context)
      : io_context_{io_context} {}

  virtual void PostDelayedTask(Duration delay, Task task) override {
    ::PostDelayedTask(io_context_, delay, std::move(task));
  }

  virtual size_t GetTaskCount() const override { return 0; }

 private:
  boost::asio::io_context& io_context_;
};
