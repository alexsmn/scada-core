#include "base/awaitable.h"

#include "base/test/test_executor.h"

#include <boost/asio/io_context.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <gtest/gtest.h>
#include <stdexcept>

using namespace std::chrono_literals;

namespace {

TEST(CoSpawn, DefersCoroutineFactoryUntilExecutorRuns) {
  auto executor = std::make_shared<TestExecutor>();

  bool factory_called = false;
  bool coroutine_started = false;

  CoSpawn(executor, [&]() -> Awaitable<void> {
    factory_called = true;
    coroutine_started = true;
    EXPECT_TRUE(executor->is_current_executor());
    co_return;
  });

  EXPECT_FALSE(factory_called);
  EXPECT_FALSE(coroutine_started);

  executor->Poll();

  EXPECT_TRUE(factory_called);
  EXPECT_TRUE(coroutine_started);
}

TEST(CoSpawn, SupportsMoveOnlyCoroutineFactoryCaptures) {
  auto executor = std::make_shared<TestExecutor>();
  auto value = std::make_unique<int>(42);

  int observed = 0;

  CoSpawn(executor,
          [owned = std::move(value), &observed]() -> Awaitable<void> {
            observed = *owned;
            co_return;
          });

  EXPECT_EQ(value, nullptr);
  EXPECT_EQ(observed, 0);

  executor->Poll();

  EXPECT_EQ(observed, 42);
}

TEST(CoSpawn, DefersCoroutineFactoryUntilAnyExecutorRuns) {
  auto executor = std::make_shared<TestExecutor>();
  auto any_executor = MakeTestAnyExecutor(executor);

  bool factory_called = false;
  bool coroutine_started = false;

  CoSpawn(any_executor, [&]() -> Awaitable<void> {
    factory_called = true;
    coroutine_started = true;
    EXPECT_TRUE(executor->is_current_executor());
    co_return;
  });

  EXPECT_FALSE(factory_called);
  EXPECT_FALSE(coroutine_started);

  executor->Poll();

  EXPECT_TRUE(factory_called);
  EXPECT_TRUE(coroutine_started);
}

TEST(CoSpawn, SupportsMoveOnlyCoroutineFactoryCapturesWithAnyExecutor) {
  auto executor = std::make_shared<TestExecutor>();
  auto any_executor = MakeTestAnyExecutor(executor);
  auto value = std::make_unique<int>(42);

  int observed = 0;

  CoSpawn(any_executor,
          [owned = std::move(value), &observed]() -> Awaitable<void> {
            observed = *owned;
            co_return;
          });

  EXPECT_EQ(value, nullptr);
  EXPECT_EQ(observed, 0);

  executor->Poll();

  EXPECT_EQ(observed, 42);
}

TEST(CoSpawn, WeakPtrFactoryReceivesLockedSharedPtrWhenAlive) {
  auto executor = std::make_shared<TestExecutor>();
  auto alive = std::make_shared<int>(42);

  bool factory_called = false;
  int observed = 0;

  CoSpawn(executor, std::weak_ptr{alive},
          [&](std::shared_ptr<int> locked) -> Awaitable<void> {
            factory_called = true;
            observed = *locked;
            co_return;
          });

  EXPECT_FALSE(factory_called);
  EXPECT_EQ(observed, 0);

  executor->Poll();

  EXPECT_TRUE(factory_called);
  EXPECT_EQ(observed, 42);
}

TEST(CoSpawn, WeakPtrFactoryDoesNotRunAfterCancelationExpires) {
  auto executor = std::make_shared<TestExecutor>();
  auto alive = std::make_shared<int>(42);

  bool factory_called = false;

  CoSpawn(executor, std::weak_ptr{alive}, [&]() -> Awaitable<void> {
    factory_called = true;
    co_return;
  });

  alive.reset();
  executor->Poll();

  EXPECT_FALSE(factory_called);
}

TEST(CoSpawn, CancelationFactoryDoesNotRunAfterCancelationCancels) {
  auto executor = std::make_shared<TestExecutor>();
  Cancelation cancelation;

  bool factory_called = false;

  CoSpawn(executor, cancelation, [&]() -> Awaitable<void> {
    factory_called = true;
    co_return;
  });

  cancelation.Cancel();
  executor->Poll();

  EXPECT_FALSE(factory_called);
}

TEST(RunAwaitable, RunsIoContextUntilAwaitableCompletes) {
  boost::asio::io_context io_context;

  bool after_timer = false;
  auto result = RunAwaitable(io_context, [&]() -> Awaitable<int> {
    boost::asio::steady_timer timer{io_context};
    timer.expires_after(1ms);
    co_await timer.async_wait(boost::asio::use_awaitable);
    after_timer = true;
    co_return 42;
  });

  EXPECT_TRUE(after_timer);
  EXPECT_EQ(result, 42);
}

TEST(RunAwaitable, RestartsStoppedIoContext) {
  boost::asio::io_context io_context;
  io_context.stop();

  bool ran = false;
  RunAwaitable(io_context, [&]() -> Awaitable<void> {
    ran = true;
    co_return;
  });

  EXPECT_TRUE(ran);
}

TEST(RunAwaitable, PropagatesAwaitableExceptions) {
  boost::asio::io_context io_context;

  EXPECT_THROW(
      RunAwaitable(io_context, []() -> Awaitable<void> {
        throw std::runtime_error{"run-awaitable-failed"};
        co_return;
      }),
      std::runtime_error);
}

}  // namespace
