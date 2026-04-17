#include "base/awaitable_promise.h"

#include "base/test/asio_test_environment.h"

#include <boost/asio/use_future.hpp>
#include <gtest/gtest.h>

using namespace std::chrono_literals;

namespace {

promise<int> MakeDeferredValuePromise(const std::shared_ptr<Executor>& executor,
                                      int value) {
  promise<void> trigger;
  promise<int> result;

  Dispatch(*executor, [trigger]() mutable { trigger.resolve(); });
  trigger.then([result, value]() mutable { result.resolve(value); });

  return result;
}

promise<void> MakeDeferredVoidPromise(const std::shared_ptr<Executor>& executor) {
  promise<void> trigger;
  promise<void> result;

  Dispatch(*executor, [trigger]() mutable { trigger.resolve(); });
  trigger.then([result]() mutable { result.resolve(); });

  return result;
}

template <class T>
T WaitForFuture(AsioTestEnvironment& asio_env, std::future<T>& result) {
  while (result.wait_for(0ms) != std::future_status::ready) {
    asio_env.io_context.run_for(10ms);
    asio_env.io_context.restart();
  }
  return result.get();
}

}  // namespace

TEST(AwaitPromise, ReturnsResolvedValue) {
  AsioTestEnvironment asio_env;

  auto result = boost::asio::co_spawn(
      asio_env.io_context,
      AwaitPromise(NetExecutorAdapter{asio_env.executor},
                   make_resolved_promise(42)),
      boost::asio::use_future);
  EXPECT_EQ(WaitForFuture(asio_env, result), 42);
}

TEST(AwaitPromise, CompletesDeferredTemporaryValuePromise) {
  AsioTestEnvironment asio_env;

  auto result = boost::asio::co_spawn(
      asio_env.io_context,
      AwaitPromise(NetExecutorAdapter{asio_env.executor},
                   MakeDeferredValuePromise(asio_env.executor, 42)),
      boost::asio::use_future);
  EXPECT_EQ(WaitForFuture(asio_env, result), 42);
}

TEST(ToAwaitable, CompletesDeferredTemporaryVoidPromise) {
  AsioTestEnvironment asio_env;

  auto result = boost::asio::co_spawn(
      asio_env.io_context,
      [](
          const std::shared_ptr<Executor>& executor) -> Awaitable<void> {
        co_await ToAwaitable(MakeDeferredVoidPromise(executor));
      }(asio_env.executor),
      boost::asio::use_future);
  EXPECT_NO_THROW(WaitForFuture(asio_env, result));
}
