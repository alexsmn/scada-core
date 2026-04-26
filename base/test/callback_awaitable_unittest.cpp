#include "base/callback_awaitable.h"

#include "base/test/awaitable_test.h"
#include "base/test/test_executor.h"

#include <gtest/gtest.h>
#include <thread>

TEST(CallbackToAwaitable, CompletesWithCallbackValues) {
  auto executor = std::make_shared<TestExecutor>();

  EXPECT_EQ(
      WaitAwaitable(executor, [executor]() -> Awaitable<int> {
        auto [value] = co_await CallbackToAwaitable<int>(
            executor, [](auto callback) mutable { callback(42); });
        co_return value;
      }()),
      42);
}

TEST(CallbackToAwaitable, PreservesFailureLikeResults) {
  auto executor = std::make_shared<TestExecutor>();

  EXPECT_EQ(
      WaitAwaitable(executor, [executor]() -> Awaitable<std::tuple<bool, int>> {
        co_return co_await CallbackToAwaitable<bool, int>(
            executor, [](auto callback) mutable { callback(false, 7); });
      }()),
      std::make_tuple(false, 7));
}

TEST(CallbackToAwaitable, ResumesOnBoundExecutorWhenCallbackRunsOffExecutor) {
  auto executor = std::make_shared<TestExecutor>();

  EXPECT_NO_THROW(WaitAwaitable(executor, [executor]() -> Awaitable<void> {
        auto [value] = co_await CallbackToAwaitable<int>(
            executor, [](auto callback) mutable {
              std::thread worker{[callback = std::move(callback)]() mutable {
                callback(42);
              }};
              worker.join();
            });

        EXPECT_EQ(value, 42);
        EXPECT_TRUE(executor->is_current_executor());
      }()));
}

TEST(CallbackToAwaitable, CompletesWithCallbackValuesOnAnyExecutor) {
  auto executor = std::make_shared<TestExecutor>();
  auto any_executor = MakeTestAnyExecutor(executor);

  EXPECT_EQ(
      WaitAwaitable(executor, [any_executor]() mutable -> Awaitable<int> {
        auto [value] = co_await CallbackToAwaitable<int>(
            std::move(any_executor),
            [](auto callback) mutable { callback(42); });
        co_return value;
      }()),
      42);
}

TEST(CallbackToAwaitable,
     ResumesOnBoundAnyExecutorWhenCallbackRunsOffExecutor) {
  auto executor = std::make_shared<TestExecutor>();
  auto any_executor = MakeTestAnyExecutor(executor);

  EXPECT_NO_THROW(WaitAwaitable(
      executor, [executor, any_executor]() mutable -> Awaitable<void> {
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
      }()));
}
