#pragma once

#include "base/executor.h"

#include <boost/asio/io_context.hpp>
#include <boost/asio/io_context_strand.hpp>

class AsioStrandExecutor : public Executor {
 public:
  explicit AsioStrandExecutor(boost::asio::io_context& io_context)
      : io_context_{io_context} {}

  virtual void PostDelayedTask(Duration delay, Task task) override {
    strand_.post(task);
  }

  virtual size_t GetTaskCount() const override { return 0; }

 private:
  boost::asio::io_context& io_context_;
  boost::asio::io_context::strand strand_{io_context_};
};