#include "base/any_executor_dispatch.h"
#include "base/executor.h"
#include "base/executor_conversions.h"
#include "base/test/future_test.h"
#include "base/test/test_executor.h"

#include <gtest/gtest.h>

#include <future>
#include <thread>

namespace {

TEST(ExecutorConversions, LegacyExecutorDispatchesOnWrappedAnyExecutor) {
  auto executor = std::make_shared<TestExecutor>();
  auto legacy_executor = MakeLegacyExecutor(MakeTestAnyExecutor(executor));

  bool called = false;
  Dispatch(*legacy_executor, [&] {
    called = true;
    EXPECT_TRUE(executor->is_current_executor());
  });

  EXPECT_FALSE(called);
  executor->Poll();
  EXPECT_TRUE(called);
}

TEST(ExecutorConversions, AnyExecutorDispatchesOnWrappedLegacyExecutor) {
  auto executor = std::make_shared<TestExecutor>();
  auto any_executor = MakeAnyExecutor(executor);

  bool called = false;
  Dispatch(any_executor, [&] {
    called = true;
    EXPECT_TRUE(executor->is_current_executor());
  });

  EXPECT_FALSE(called);
  executor->Poll();
  EXPECT_TRUE(called);
}

TEST(ExecutorConversions, ThreadAnyExecutorRunsTaskOnWorkerThread) {
  auto executor = MakeThreadAnyExecutor();

  std::promise<std::thread::id> result;
  auto future = result.get_future();

  Dispatch(executor, [&] {
    result.set_value(std::this_thread::get_id());
  });

  EXPECT_NE(WaitFuture(future), std::this_thread::get_id());
}

}  // namespace
