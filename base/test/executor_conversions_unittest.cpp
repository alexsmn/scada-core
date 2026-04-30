#include "base/any_executor_dispatch.h"
#include "base/executor.h"
#include "base/executor_conversions.h"
#include "base/test/test_executor.h"

#include <gtest/gtest.h>

#include <condition_variable>
#include <mutex>
#include <optional>
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

  std::mutex mutex;
  std::condition_variable cv;
  std::optional<std::thread::id> result;

  Dispatch(executor, [&] {
    {
      std::lock_guard lock{mutex};
      result = std::this_thread::get_id();
    }
    cv.notify_all();
  });

  std::unique_lock lock{mutex};
  cv.wait(lock, [&] { return result.has_value(); });
  EXPECT_NE(*result, std::this_thread::get_id());
}

}  // namespace
