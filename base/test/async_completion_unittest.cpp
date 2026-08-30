#include "base/async_completion.h"

#include "base/test/awaitable_test.h"

#include <gtest/gtest.h>

#include <memory>
#include <stdexcept>

using namespace std::chrono_literals;

TEST(AsyncCompletion, WaitersResumeWhenCompleted) {
  TestExecutor executor;
  scada::base::AsyncCompletion completion{executor};

  auto first = StartAwaitable(executor, completion.Wait());
  auto second = StartAwaitable(executor, completion.Wait());

  Drain(executor);
  EXPECT_FALSE(first->done);
  EXPECT_FALSE(second->done);

  completion.Complete();

  EXPECT_NO_THROW(WaitResult(executor, first));
  EXPECT_NO_THROW(WaitResult(executor, second));
}

TEST(AsyncCompletion, WaitAfterCompleteReturnsImmediately) {
  TestExecutor executor;
  scada::base::AsyncCompletion completion{executor};

  completion.Complete();

  EXPECT_NO_THROW(WaitAwaitable(executor, completion.Wait()));
}

// `Complete()` panics on a second call, which is the right contract for a gate
// one owner settles exactly once and the wrong one where two parties race to
// settle it -- a deadline against the work it bounds. `TryComplete()` is that
// second case, and says which call won.
TEST(AsyncCompletion, TryCompleteSettlesTheGateOnce) {
  TestExecutor executor;
  scada::base::AsyncCompletion completion{executor};

  auto waiter = StartAwaitable(executor, completion.Wait());

  EXPECT_TRUE(completion.TryComplete());
  EXPECT_TRUE(completion.completed());
  EXPECT_NO_THROW(WaitResult(executor, waiter));

  // The losing party -- a branch settling after the deadline released the
  // waiter -- must not abort the process.
  EXPECT_FALSE(completion.TryComplete());
  EXPECT_TRUE(completion.completed());
}

TEST(AsyncCompletion, TryCompleteAfterFailureKeepsTheFailure) {
  TestExecutor executor;
  scada::base::AsyncCompletion completion{executor};

  completion.Fail(std::make_exception_ptr(std::runtime_error{"boom"}));

  EXPECT_FALSE(completion.TryComplete());
  EXPECT_THROW(WaitAwaitable(executor, completion.Wait()), std::runtime_error);
}

TEST(AsyncCompletion, CopiesShareCompletionState) {
  TestExecutor executor;
  scada::base::AsyncCompletion owner{executor};
  auto handle = owner;

  auto waiter = StartAwaitable(executor, owner.Wait());
  Drain(executor);
  EXPECT_FALSE(waiter->done);

  handle.Complete();

  EXPECT_NO_THROW(WaitResult(executor, waiter));
  EXPECT_TRUE(owner.completed());
  EXPECT_NO_THROW(WaitAwaitable(executor, handle.Wait()));
}

TEST(AsyncCompletion, FailurePropagatesToCurrentAndFutureWaiters) {
  TestExecutor executor;
  scada::base::AsyncCompletion completion{executor};

  auto waiter = StartAwaitable(executor, completion.Wait());
  Drain(executor);
  EXPECT_FALSE(waiter->done);

  completion.Fail(std::make_exception_ptr(std::runtime_error{"failed"}));

  EXPECT_THROW(WaitResult(executor, waiter), std::runtime_error);
  EXPECT_THROW(WaitAwaitable(executor, completion.Wait()), std::runtime_error);
}

TEST(AsyncCompletion, PrecreatedWaitDoesNotDependOnOwnerLifetime) {
  TestExecutor executor;
  auto completion = std::make_unique<scada::base::AsyncCompletion>(executor);

  auto waiter = completion->Wait();
  completion->Complete();
  completion.reset();

  EXPECT_NO_THROW(WaitAwaitable(executor, std::move(waiter)));
}
