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

  boost::asio::io_context& io_context() { return core_->io_context; }

  template <class Handler>
  void post(Handler&& handler) {
    core_->io_context.post(std::forward<Handler>(handler));
  }

  template <class Handler>
  void dispatch(Handler&& handler) {
    core_->io_context.dispatch(std::forward<Handler>(handler));
  }

  template <class Handler>
  auto wrap(Handler&& handler) {
    return core_->io_context.wrap(std::forward<Handler>(handler));
  }

 private:
  struct Core {
    boost::asio::io_context io_context{1};
    std::optional<boost::asio::io_context::work> work{io_context};
  };

  const std::shared_ptr<Core> core_ = std::make_shared<Core>();
  std::thread thread_;
};

inline SingleThreadExecutor::SingleThreadExecutor() {
  thread_ = std::thread{[core = core_] { core->io_context.run(); }};
}

inline SingleThreadExecutor::~SingleThreadExecutor() {
  core_->work.reset();

  if (thread_.get_id() == std::this_thread::get_id())
    thread_.detach();
  else
    thread_.join();
}
