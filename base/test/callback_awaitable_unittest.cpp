#include "base/callback_awaitable.h"

#include "base/test/awaitable_test.h"
#include "base/test/test_executor.h"

#include <boost/asio/bind_cancellation_slot.hpp>
#include <boost/asio/cancellation_signal.hpp>
#include <boost/asio/cancellation_type.hpp>
#include <gtest/gtest.h>
#include <thread>

TEST(CallbackToAwaitable, CompletesWithCallbackValues) {
  TestExecutor executor;

  EXPECT_EQ(WaitAwaitable(executor,
                          [executor]() -> Awaitable<int> {
                            auto [value] = co_await CallbackToAwaitable<int>(
                                executor,
                                [](auto callback) mutable { callback(42); });
                            co_return value;
                          }()),
            42);
}

TEST(CallbackToAwaitable, PreservesFailureLikeResults) {
  TestExecutor executor;

  EXPECT_EQ(WaitAwaitable(executor,
                          [executor]() -> Awaitable<std::tuple<bool, int>> {
                            co_return co_await CallbackToAwaitable<bool, int>(
                                executor, [](auto callback) mutable {
                                  callback(false, 7);
                                });
                          }()),
            std::make_tuple(false, 7));
}

TEST(CallbackToAwaitable, ResumesOnBoundExecutorWhenCallbackRunsOffExecutor) {
  TestExecutor executor;

  EXPECT_NO_THROW(WaitAwaitable(executor, [executor]() -> Awaitable<void> {
    auto [value] =
        co_await CallbackToAwaitable<int>(executor, [](auto callback) mutable {
          std::thread worker{
              [callback = std::move(callback)]() mutable { callback(42); }};
          worker.join();
        });

    EXPECT_EQ(value, 42);
    EXPECT_TRUE(executor.is_current_executor());
  }()));
}

TEST(CallbackToAwaitable, CompletesWithCallbackValuesOnAnyExecutor) {
  TestExecutor executor;
  auto any_executor = executor;

  EXPECT_EQ(WaitAwaitable(executor,
                          [any_executor]() mutable -> Awaitable<int> {
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
            std::move(any_executor), [](auto callback) mutable {
              std::thread worker{
                  [callback = std::move(callback)]() mutable { callback(42); }};
              worker.join();
            });

        EXPECT_EQ(value, 42);
        EXPECT_TRUE(executor.is_current_executor());
      }()));
}

// A `start` that takes the handler's cancellation slot as well as the callback
// is handed it, and owns what cancellation means: here it completes with -1.
// Before the slot was passed at all, the awaitable ignored cancellation and
// this waited for ever.
TEST(CallbackToAwaitable, HandsTheHandlersCancellationSlotToAStartThatTakesIt) {
  TestExecutor executor;
  boost::asio::cancellation_signal cancel;

  auto result =
      StartAwaitable(executor, [executor, &cancel]() -> Awaitable<int> {
        auto [value] = co_await CallbackToAwaitable<int>(
            executor,
            [](auto callback, boost::asio::cancellation_slot slot) mutable {
              EXPECT_TRUE(slot.is_connected());
              slot.assign([callback](boost::asio::cancellation_type) mutable {
                callback(-1);
              });
            },
            boost::asio::bind_cancellation_slot(cancel.slot(),
                                                boost::asio::use_awaitable));
        co_return value;
      }());

  Drain(executor);
  EXPECT_FALSE(result->done) << "nothing has cancelled the wait yet";

  cancel.emit(boost::asio::cancellation_type::terminal);

  EXPECT_EQ(WaitResult(executor, result), -1);
}

// An awaiter with no cancellation slot connected -- the ordinary `co_await`
// from a detached coroutine -- hands over a disconnected slot, so a `start`
// that takes one can tell there is nothing to honour.
TEST(CallbackToAwaitable, ADisconnectedSlotIsReportedAsSuch) {
  TestExecutor executor;

  EXPECT_EQ(
      WaitAwaitable(executor,
                    [executor]() -> Awaitable<bool> {
                      auto [connected] = co_await CallbackToAwaitable<bool>(
                          executor,
                          [](auto callback,
                             boost::asio::cancellation_slot slot) mutable {
                            callback(slot.is_connected());
                          });
                      co_return connected;
                    }()),
      false);
}
