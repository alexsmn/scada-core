#include "base/callback_awaitable.h"

#include "base/test/awaitable_test.h"
#include "base/test/test_executor.h"

#include <gtest/gtest.h>
#include <thread>

TEST(CallbackToAwaitable, CompletesWithCallbackValues) {
  TestExecutor executor;

  EXPECT_EQ(
      WaitAwaitable(executor, [executor]() -> Awaitable<int> {
        auto [value] = co_await CallbackToAwaitable<int>(
            executor,
            [](auto callback) mutable { callback(42); });
        co_return value;
      }()),
      42);
}

TEST(CallbackToAwaitable, PreservesFailureLikeResults) {
  TestExecutor executor;

  EXPECT_EQ(
      WaitAwaitable(executor, [executor]() -> Awaitable<std::tuple<bool, int>> {
        co_return co_await CallbackToAwaitable<bool, int>(
            executor,
            [](auto callback) mutable { callback(false, 7); });
      }()),
      std::make_tuple(false, 7));
}

TEST(CallbackToAwaitable, ResumesOnBoundExecutorWhenCallbackRunsOffExecutor) {
  TestExecutor executor;

  EXPECT_NO_THROW(WaitAwaitable(executor, [executor]() -> Awaitable<void> {
        auto [value] = co_await CallbackToAwaitable<int>(
            executor, [](auto callback) mutable {
              std::thread worker{[callback = std::move(callback)]() mutable {
                callback(42);
              }};
              worker.join();
            });

        EXPECT_EQ(value, 42);
        EXPECT_TRUE(executor.is_current_executor());
      }()));
}

TEST(CallbackToAwaitable, CompletesWithCallbackValuesOnAnyExecutor) {
  TestExecutor executor;
  auto any_executor = executor;

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
  TestExecutor executor;
  auto any_executor = executor;

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
        EXPECT_TRUE(executor.is_current_executor());
      }()));
}
