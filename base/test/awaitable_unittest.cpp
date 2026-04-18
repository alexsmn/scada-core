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

}  // namespace
