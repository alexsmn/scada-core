#include "remote/remote_listener.h"

#include "base/boost_log.h"
#include "base/test/asio_test_environment.h"

#include <boost/asio/steady_timer.hpp>
#include <gmock/gmock.h>
#include <transport/any_transport.h>

using namespace std::chrono_literals;
using namespace testing;

namespace {

class BlockingAcceptorTransport : public transport::Transport {
 public:
  explicit BlockingAcceptorTransport(transport::executor executor)
      : executor_{std::move(executor)} {}

  transport::awaitable<transport::error_code> open() override {
    opened_ = true;
    co_return transport::OK;
  }

  transport::awaitable<transport::error_code> close() override {
    closed_ = true;
    co_return transport::OK;
  }

  transport::awaitable<transport::expected<transport::any_transport>> accept()
      override {
    while (!closed_) {
      boost::asio::steady_timer timer{executor_};
      timer.expires_after(1ms);
      co_await timer.async_wait(boost::asio::use_awaitable);
    }
    co_return transport::ERR_ABORTED;
  }

  transport::awaitable<transport::expected<size_t>> read(
      std::span<char>) override {
    co_return transport::ERR_NOT_IMPLEMENTED;
  }

  transport::awaitable<transport::expected<size_t>> write(
      std::span<const char>) override {
    co_return transport::ERR_NOT_IMPLEMENTED;
  }

  std::string name() const override { return "BlockingAcceptor"; }
  bool message_oriented() const override { return true; }
  bool connected() const override { return opened_ && !closed_; }
  bool active() const override { return false; }
  transport::executor get_executor() override { return executor_; }

 private:
  transport::executor executor_;
  bool opened_ = false;
  bool closed_ = false;
};

class FailingOpenTransport final : public BlockingAcceptorTransport {
 public:
  using BlockingAcceptorTransport::BlockingAcceptorTransport;

  transport::awaitable<transport::error_code> open() override {
    co_return transport::ERR_FAILED;
  }
};

std::shared_ptr<RemoteListener> CreateListener(
    AsioTestEnvironment& env,
    std::unique_ptr<transport::Transport> transport,
    RemoteListener::AcceptHandler accept_handler =
        [](transport::any_transport) {}) {
  return RemoteListener::Create(
      std::make_shared<BoostLogger>(LOG_NAME("RemoteListenerTest")),
      transport::any_transport{std::move(transport)}, "test-listener",
      std::move(accept_handler));
}

}  // namespace

TEST(RemoteListenerTest, InitAsyncCompletesWhenAcceptorOpens) {
  AsioTestEnvironment env;
  auto listener = CreateListener(
      env,
      std::make_unique<BlockingAcceptorTransport>(env.any_executor_factory()));

  EXPECT_NO_THROW(env.Wait(listener->InitAsync()));

  EXPECT_NO_THROW(env.Wait(listener->ShutdownAsync()));
}

TEST(RemoteListenerTest, InitAsyncThrowsWhenAcceptorOpenFails) {
  AsioTestEnvironment env;
  auto listener = CreateListener(
      env, std::make_unique<FailingOpenTransport>(env.any_executor_factory()));

  EXPECT_THROW(env.Wait(listener->InitAsync()), std::runtime_error);
  EXPECT_NO_THROW(env.Wait(listener->ShutdownAsync()));
}
