#include "base/awaitable.h"

#include "base/test/test_executor.h"

#include <gtest/gtest.h>

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

}  // namespace
