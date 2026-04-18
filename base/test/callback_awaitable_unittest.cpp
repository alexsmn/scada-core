#include "base/callback_awaitable.h"

#include "base/awaitable.h"
#include "base/executor_adapter.h"
#include "base/test/test_executor.h"

#include <boost/asio/use_future.hpp>
#include <gtest/gtest.h>
#include <thread>

using namespace std::chrono_literals;

namespace {

template <class T>
T WaitForFuture(std::shared_ptr<TestExecutor> executor, std::future<T>& future) {
  while (future.wait_for(0ms) != std::future_status::ready) {
    executor->Poll();
  }
  return future.get();
}

void WaitForFuture(std::shared_ptr<TestExecutor> executor,
                   std::future<void>& future) {
  while (future.wait_for(0ms) != std::future_status::ready) {
    executor->Poll();
  }
  future.get();
}

}  // namespace

TEST(CallbackToAwaitable, CompletesWithCallbackValues) {
  auto executor = std::make_shared<TestExecutor>();

  auto future = boost::asio::co_spawn(
      ExecutorAdapter{executor},
      [executor]() -> Awaitable<int> {
        auto [value] = co_await CallbackToAwaitable<int>(
            executor, [](auto callback) mutable { callback(42); });
        co_return value;
      }(),
      boost::asio::use_future);

  EXPECT_EQ(WaitForFuture(executor, future), 42);
}

TEST(CallbackToAwaitable, PreservesFailureLikeResults) {
  auto executor = std::make_shared<TestExecutor>();

  auto future = boost::asio::co_spawn(
      ExecutorAdapter{executor},
      [executor]() -> Awaitable<std::tuple<bool, int>> {
        co_return co_await CallbackToAwaitable<bool, int>(
            executor, [](auto callback) mutable { callback(false, 7); });
      }(),
      boost::asio::use_future);

  EXPECT_EQ(WaitForFuture(executor, future), std::make_tuple(false, 7));
}

TEST(CallbackToAwaitable, ResumesOnBoundExecutorWhenCallbackRunsOffExecutor) {
  auto executor = std::make_shared<TestExecutor>();

  auto future = boost::asio::co_spawn(
      ExecutorAdapter{executor},
      [executor]() -> Awaitable<void> {
        auto [value] = co_await CallbackToAwaitable<int>(
            executor, [](auto callback) mutable {
              std::thread worker{[callback = std::move(callback)]() mutable {
                callback(42);
              }};
              worker.join();
            });

        EXPECT_EQ(value, 42);
        EXPECT_TRUE(executor->is_current_executor());
      }(),
      boost::asio::use_future);

  EXPECT_NO_THROW(WaitForFuture(executor, future));
}

TEST(CallbackToAwaitable, CompletesWithCallbackValuesOnAnyExecutor) {
  auto executor = std::make_shared<TestExecutor>();
  auto any_executor = MakeTestAnyExecutor(executor);

  auto future = boost::asio::co_spawn(
      ExecutorAdapter{executor},
      [any_executor]() mutable -> Awaitable<int> {
        auto [value] = co_await CallbackToAwaitable<int>(
            std::move(any_executor),
            [](auto callback) mutable { callback(42); });
        co_return value;
      }(),
      boost::asio::use_future);

  EXPECT_EQ(WaitForFuture(executor, future), 42);
}

TEST(CallbackToAwaitable,
     ResumesOnBoundAnyExecutorWhenCallbackRunsOffExecutor) {
  auto executor = std::make_shared<TestExecutor>();
  auto any_executor = MakeTestAnyExecutor(executor);

  auto future = boost::asio::co_spawn(
      ExecutorAdapter{executor},
      [executor, any_executor]() mutable -> Awaitable<void> {
        auto [value] = co_await CallbackToAwaitable<int>(
            std::move(any_executor),
            [](auto callback) mutable {
              std::thread worker{[callback = std::move(callback)]() mutable {
                callback(42);
              }};
              worker.join();
            });

        EXPECT_EQ(value, 42);
        EXPECT_TRUE(executor->is_current_executor());
      }(),
      boost::asio::use_future);

  EXPECT_NO_THROW(WaitForFuture(executor, future));
}
