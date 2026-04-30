#include "base/thread_executor.h"

#include <gmock/gmock.h>
#include <condition_variable>
#include <mutex>

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
  std::mutex mutex;
  std::condition_variable cv;
  bool release = false;
  executor->PostTask([executor, &mutex, &cv, &release] {
    std::unique_lock lock{mutex};
    cv.wait(lock, [&] { return release; });
  });
  executor = nullptr;
  {
    std::lock_guard lock{mutex};
    release = true;
  }
  cv.notify_all();
}
