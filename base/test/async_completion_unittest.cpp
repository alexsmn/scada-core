#include "base/async_completion.h"

#include "base/awaitable_promise.h"
#include "base/test/awaitable_test.h"

#include <gtest/gtest.h>

#include <stdexcept>

using namespace std::chrono_literals;

TEST(AsyncCompletion, WaitersResumeWhenCompleted) {
  auto executor = std::make_shared<TestExecutor>();
  base::AsyncCompletion completion{MakeTestAnyExecutor(executor)};

  auto first = ToPromise(MakeTestAnyExecutor(executor), completion.Wait());
  auto second = ToPromise(MakeTestAnyExecutor(executor), completion.Wait());

  Drain(executor);
  EXPECT_EQ(first.wait_for(0ms), promise_wait_status::timeout);
  EXPECT_EQ(second.wait_for(0ms), promise_wait_status::timeout);

  completion.Complete();

  EXPECT_NO_THROW(WaitPromise(executor, std::move(first)));
  EXPECT_NO_THROW(WaitPromise(executor, std::move(second)));
}

TEST(AsyncCompletion, WaitAfterCompleteReturnsImmediately) {
  auto executor = std::make_shared<TestExecutor>();
  base::AsyncCompletion completion{MakeTestAnyExecutor(executor)};

  completion.Complete();

  EXPECT_NO_THROW(WaitAwaitable(executor, completion.Wait()));
}

TEST(AsyncCompletion, FailurePropagatesToCurrentAndFutureWaiters) {
  auto executor = std::make_shared<TestExecutor>();
  base::AsyncCompletion completion{MakeTestAnyExecutor(executor)};

  auto waiter = ToPromise(MakeTestAnyExecutor(executor), completion.Wait());
  Drain(executor);
  EXPECT_EQ(waiter.wait_for(0ms), promise_wait_status::timeout);

  completion.Fail(std::make_exception_ptr(std::runtime_error{"failed"}));

  EXPECT_THROW(WaitPromise(executor, std::move(waiter)), std::runtime_error);
  EXPECT_THROW(WaitAwaitable(executor, completion.Wait()), std::runtime_error);
}
