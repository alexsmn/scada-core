#include "base/awaitable_completion.h"

#include "base/awaitable.h"
#include "base/promise.h"
#include "base/test/awaitable_test.h"
#include "base/test/test_executor.h"

#include <gtest/gtest.h>
#include <thread>

using namespace std::chrono_literals;

namespace {

TEST(AwaitableCompletion, ResolvedCompletionReturnsImmediately) {
  auto executor = std::make_shared<TestExecutor>();
  auto completion = AwaitableCompletion::Resolved();

  EXPECT_NO_THROW(
      WaitAwaitable(executor, completion.Await(MakeTestAnyExecutor(executor))));
}

TEST(AwaitableCompletion, PendingCompletionResumesAllWaitersWhenCompleted) {
  auto executor = std::make_shared<TestExecutor>();
  AwaitableCompletion completion;
  auto first = std::make_shared<promise<int>>();
  auto second = std::make_shared<promise<int>>();

  CoSpawn(MakeTestAnyExecutor(executor),
          [executor, completion, first]() mutable -> Awaitable<void> {
            try {
              co_await completion.Await(MakeTestAnyExecutor(executor));
              first->resolve(1);
            } catch (...) {
              first->reject(std::current_exception());
            }
          });

  CoSpawn(MakeTestAnyExecutor(executor),
          [executor, completion, second]() mutable -> Awaitable<void> {
            try {
              co_await completion.Await(MakeTestAnyExecutor(executor));
              second->resolve(2);
            } catch (...) {
              second->reject(std::current_exception());
            }
          });

  executor->Poll();
  EXPECT_EQ(first->wait_for(0ms), promise_wait_status::timeout);
  EXPECT_EQ(second->wait_for(0ms), promise_wait_status::timeout);

  completion.Complete();

  EXPECT_EQ(WaitPromise(executor, first), 1);
  EXPECT_EQ(WaitPromise(executor, second), 2);
}

TEST(AwaitableCompletion, CompletionFailurePropagatesExceptionToAllWaiters) {
  auto executor = std::make_shared<TestExecutor>();
  AwaitableCompletion completion;
  auto first = std::make_shared<promise<void>>();
  auto second = std::make_shared<promise<void>>();

  CoSpawn(MakeTestAnyExecutor(executor),
          [executor, completion, first]() mutable -> Awaitable<void> {
            try {
              co_await completion.Await(MakeTestAnyExecutor(executor));
              first->resolve();
            } catch (...) {
              first->reject(std::current_exception());
            }
          });

  CoSpawn(MakeTestAnyExecutor(executor),
          [executor, completion, second]() mutable -> Awaitable<void> {
            try {
              co_await completion.Await(MakeTestAnyExecutor(executor));
              second->resolve();
            } catch (...) {
              second->reject(std::current_exception());
            }
          });

  executor->Poll();

  completion.Fail(std::make_exception_ptr(std::runtime_error{"boom"}));

  EXPECT_THROW(WaitPromise(executor, first), std::runtime_error);
  EXPECT_THROW(WaitPromise(executor, second), std::runtime_error);
}

TEST(AwaitableCompletion, ResumesOnBoundExecutorWhenCompletedOffExecutor) {
  auto executor = std::make_shared<TestExecutor>();
  AwaitableCompletion completion;
  auto result = std::make_shared<promise<void>>();

  CoSpawn(MakeTestAnyExecutor(executor),
          [executor, completion, result]() mutable -> Awaitable<void> {
            try {
              co_await completion.Await(MakeTestAnyExecutor(executor));
              EXPECT_TRUE(executor->is_current_executor());
              result->resolve();
            } catch (...) {
              result->reject(std::current_exception());
            }
          });

  executor->Poll();

  std::thread worker{[completion]() mutable { completion.Complete(); }};
  worker.join();

  EXPECT_NO_THROW(WaitPromise(executor, result));
}

}  // namespace
