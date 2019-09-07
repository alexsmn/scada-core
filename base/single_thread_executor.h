#pragma once

#include <boost/asio/io_context.hpp>
#include <optional>
#include <thread>

class SingleThreadExecutor {
 public:
  SingleThreadExecutor();
  ~SingleThreadExecutor();

  SingleThreadExecutor(const SingleThreadExecutor&) = delete;
  SingleThreadExecutor& operator=(const SingleThreadExecutor&) = delete;

  boost::asio::io_context& io_context() { return io_context_; }

  template <class Handler>
  void post(Handler&& handler) {
    io_context_.post(std::forward<Handler>(handler));
  }

  template <class Handler>
  void dispatch(Handler&& handler) {
    io_context_.dispatch(std::forward<Handler>(handler));
  }

  template <class Handler>
  auto wrap(Handler&& handler) {
    return io_context_.wrap(std::forward<Handler>(handler));
  }

 private:
  boost::asio::io_context io_context_{1};
  std::optional<boost::asio::io_context::work> io_context_work_{io_context_};
  std::thread thread_;
};

inline SingleThreadExecutor::SingleThreadExecutor() {
  thread_ = std::thread{[& io_context = io_context_] { io_context.run(); }};
}

inline SingleThreadExecutor::~SingleThreadExecutor() {
  io_context_work_.reset();
  thread_.join();
}
