#include "base/thread_executor.h"

#include <boost/asio/post.hpp>
#include <gmock/gmock.h>
#include <condition_variable>
#include <future>
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
  std::promise<void> finished;
  auto finished_future = finished.get_future();
  bool release = false;
  executor->PostTask([executor, &mutex, &cv, &release,
                      &finished]() mutable {
    std::unique_lock lock{mutex};
    cv.wait(lock, [&] { return release; });
    lock.unlock();
    executor = nullptr;
    finished.set_value();
  });
  executor = nullptr;
  {
    std::lock_guard lock{mutex};
    release = true;
  }
  cv.notify_all();
  EXPECT_EQ(finished_future.wait_for(std::chrono::seconds{5}),
            std::future_status::ready);
}

TEST(ThreadExecutorTest, CanBeStoredInAnyExecutor) {
  ThreadExecutor executor;
  AnyExecutor any_executor = executor;

  std::promise<void> called;
  auto called_future = called.get_future();
  boost::asio::post(any_executor, [&] { called.set_value(); });

  EXPECT_EQ(called_future.wait_for(std::chrono::seconds{5}),
            std::future_status::ready);
}
