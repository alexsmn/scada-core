#include "base/single_thread_executor.h"

#include "base/synchronization/waitable_event.h"

#include <gmock/gmock.h>
#include <atomic>

TEST(SingleThreadExecutor, ExecutesAllTasksOnDeletion) {
  const int kMaxCounter = 1000;
  std::atomic<int> counter = kMaxCounter;

  {
    SingleThreadExecutor executor;
    for (int i = 0; i < kMaxCounter; ++i)
      executor.post([&counter] { --counter; });
  }

  EXPECT_EQ(0, counter.load());
}

TEST(SingleThreadExecutor, CanBeDeletedFromTask) {
  auto executor = std::make_unique<SingleThreadExecutor>();
  base::WaitableEvent event{base::WaitableEvent::ResetPolicy::MANUAL,
                            base::WaitableEvent::InitialState::NOT_SIGNALED};
  executor->post([&] {
    executor.reset();
    event.Signal();
  });
  event.Wait();
}