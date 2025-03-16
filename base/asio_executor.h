#pragma once

#include "base/executor.h"
#include "base/io_context_util.h"

#include <boost/asio/bind_executor.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/io_context_strand.hpp>
#include <boost/asio/post.hpp>

class AsioExecutor : public Executor {
 public:
  explicit AsioExecutor(boost::asio::io_context& io_context)
      : io_context_{io_context} {}

  virtual void PostDelayedTask(Duration delay,
                               Task task,
                               const std::source_location& location =
                                   std::source_location::current()) override {
    if (delay <= Duration::zero()) {
      boost::asio::post(strand_, std::move(task));
    } else {
      ::PostDelayedTask(io_context_, delay,
                        boost::asio::bind_executor(strand_, std::move(task)),
                        location);
    }
  }

  virtual size_t GetTaskCount() const override { return 0; }

 private:
  boost::asio::io_context& io_context_;
  boost::asio::io_context::strand strand_{io_context_};
};
