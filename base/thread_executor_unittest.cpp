#include "base/thread_executor.h"

#include "base/promise.h"

#include <gmock/gmock.h>

using namespace testing;

TEST(ThreadExecutorTest, RunsAllTasksOnDestruction) {
  int counter = 0;
  {
    ThreadExecutor executor;
    for (int i = 0; i < 100; i++)
      executor.PostTask([&] { counter++; });
  }
  EXPECT_EQ(counter, 100);
}

TEST(ThreadExecutorTest, DestroyFromTask) {
  auto executor = std::make_shared<ThreadExecutor>();
  promise<void> release_promise;
  executor->PostTask([executor, release_promise] { release_promise.get(); });
  executor = nullptr;
  release_promise.resolve();
}
