#include "base/thread_executor.h"

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
