#include "base/async_cache.h"

#include "base/test/awaitable_test.h"

#include <gtest/gtest.h>

using namespace std::chrono_literals;

TEST(AsyncCache, WaitersShareOneStartedFetch) {
  TestExecutor executor;
  base::AsyncCache<int, int> cache{executor};
  int starts = 0;

  auto first = StartAwaitable(executor, cache.Wait(1, [&](int key) {
                           if (cache.TryStart(key)) {
                             ++starts;
                           }
                         }));
  auto second = StartAwaitable(executor, cache.Wait(1, [&](int key) {
                            if (cache.TryStart(key)) {
                              ++starts;
                            }
                          }));

  Drain(executor);
  EXPECT_EQ(starts, 1);
  EXPECT_FALSE(first->done);
  EXPECT_FALSE(second->done);

  cache.Complete(1, 42);

  EXPECT_EQ(WaitResult(executor, first), 42);
  EXPECT_EQ(WaitResult(executor, second), 42);
}

TEST(AsyncCache, CompletedValueIsReused) {
  TestExecutor executor;
  base::AsyncCache<int, int> cache{executor};
  int starts = 0;

  auto first = StartAwaitable(executor, cache.Wait(1, [&](int key) {
                           if (cache.TryStart(key)) {
                             ++starts;
                           }
                         }));
  Drain(executor);
  cache.Complete(1, 42);
  EXPECT_EQ(WaitResult(executor, first), 42);

  auto second = StartAwaitable(executor, cache.Wait(1, [&](int key) {
                            if (cache.TryStart(key)) {
                              ++starts;
                            }
                          }));

  EXPECT_EQ(WaitResult(executor, second), 42);
  EXPECT_EQ(starts, 1);
}

TEST(AsyncCache, PendingKeysExposeUnstartedWaiters) {
  TestExecutor executor;
  base::AsyncCache<int, int> cache{executor};

  auto first = StartAwaitable(executor, cache.Wait(1, [](int) {}));
  auto second = StartAwaitable(executor, cache.Wait(2, [](int) {}));

  Drain(executor);
  EXPECT_FALSE(first->done);
  EXPECT_FALSE(second->done);
  EXPECT_EQ(cache.PendingKeys(), (std::vector<int>{1, 2}));

  EXPECT_TRUE(cache.TryStart(1));
  EXPECT_TRUE(cache.TryStart(2));
  cache.Complete(1, 11);
  cache.Complete(2, 22);
  EXPECT_EQ(WaitResult(executor, first), 11);
  EXPECT_EQ(WaitResult(executor, second), 22);
}
