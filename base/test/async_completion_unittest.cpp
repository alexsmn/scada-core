#include "base/async_completion.h"

#include "base/test/awaitable_test.h"

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/post.hpp>
#include <boost/asio/steady_timer.hpp>
#include <gtest/gtest.h>

#include <chrono>
#include <memory>
#include <optional>
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

// `WaitFor` bounds the wait with a real `steady_timer` (through
// `boost::asio::cancel_after`), which the TestExecutor never fires -- see the
// note in `base/any_executor.h` -- so these run an io_context.
class AsyncCompletionDeadlineTest : public ::testing::Test {
 protected:
  // Short enough to run; orders of magnitude above the work these tests do.
  static constexpr auto kTimeout = 50ms;
  // Generous against the deadline so a loaded machine cannot fail the test,
  // and still far below what an unbounded wait would take.
  static constexpr auto kRunCap = 5s;

  AnyExecutor executor() { return AnyExecutor{context_.get_executor()}; }

  // Runs |awaitable| to completion or the cap, so a lost deadline is a failed
  // expectation rather than a suite that never returns.
  template <class T>
  std::optional<T> Run(Awaitable<T> awaitable,
                       std::chrono::steady_clock::duration run_cap = kRunCap) {
    auto value = std::make_shared<std::optional<T>>();
    auto error = std::make_shared<std::exception_ptr>();
    boost::asio::co_spawn(
        context_,
        [awaitable = std::move(awaitable), value,
         error]() mutable -> Awaitable<void> {
          try {
            value->emplace(co_await std::move(awaitable));
          } catch (...) {
            *error = std::current_exception();
          }
        },
        boost::asio::detached);

    const auto deadline = std::chrono::steady_clock::now() + run_cap;
    while (!*value && !*error && std::chrono::steady_clock::now() < deadline) {
      context_.restart();
      context_.run_for(2ms);
    }
    if (*error) {
      std::rethrow_exception(*error);
    }
    return *value;
  }

  boost::asio::io_context context_;
};

TEST_F(AsyncCompletionDeadlineTest, WaitForReturnsTrueWhenTheGateSettlesFirst) {
  scada::base::AsyncCompletion completion{executor()};
  boost::asio::post(context_, [completion] { completion.Complete(); });

  auto settled = Run(completion.WaitFor(kTimeout));

  ASSERT_TRUE(settled) << "WaitFor never returned";
  EXPECT_TRUE(*settled);
}

// The regression test for the wait being uncancellable: before the slot was
// honoured `cancel_after` emitted a cancellation nobody listened to, and this
// waited until the cap.
TEST_F(AsyncCompletionDeadlineTest,
       WaitForReturnsFalseWhenTheDeadlineFiresFirst) {
  scada::base::AsyncCompletion completion{executor()};

  auto settled = Run(completion.WaitFor(kTimeout));

  ASSERT_TRUE(settled) << "the deadline never released the waiter";
  EXPECT_FALSE(*settled);
  // The deadline released the waiter, not the gate: that is what lets the
  // owner settle it later with `TryComplete`, and what keeps other waiters
  // waiting.
  EXPECT_FALSE(completion.completed());
}

TEST_F(AsyncCompletionDeadlineTest, WaitForOnASettledGateReturnsAtOnce) {
  scada::base::AsyncCompletion completion{executor()};
  completion.Complete();

  auto settled = Run(completion.WaitFor(kTimeout));

  ASSERT_TRUE(settled);
  EXPECT_TRUE(*settled);
}

TEST_F(AsyncCompletionDeadlineTest, WaitForRethrowsTheFailure) {
  scada::base::AsyncCompletion completion{executor()};
  boost::asio::post(context_, [completion] {
    completion.Fail(std::make_exception_ptr(std::runtime_error{"boom"}));
  });

  EXPECT_THROW(Run(completion.WaitFor(kTimeout)), std::runtime_error);
}

// The de-registration is the whole difficulty of a cancellable wait: a waiter
// the deadline released must not be invoked again when the gate settles, or
// the settle resumes a coroutine frame that has long since moved on.
TEST_F(AsyncCompletionDeadlineTest,
       ASettleAfterTheDeadlineDoesNotReachTheReleasedWaiter) {
  scada::base::AsyncCompletion completion{executor()};
  auto resumed = std::make_shared<int>(0);

  auto settled = Run([completion, resumed]() -> Awaitable<bool> {
    const bool settled = co_await completion.WaitFor(kTimeout);
    ++*resumed;
    co_return settled;
  }());
  ASSERT_TRUE(settled);
  EXPECT_FALSE(*settled);
  EXPECT_EQ(*resumed, 1);

  // The owner arrives late and settles the gate. The released waiter is gone;
  // a fresh, unbounded waiter is what it resumes.
  EXPECT_TRUE(completion.TryComplete());
  auto later = Run([completion]() -> Awaitable<bool> {
    co_await completion.Wait();
    co_return true;
  }());
  ASSERT_TRUE(later);
  EXPECT_EQ(*resumed, 1) << "the released waiter was resumed a second time";
}

// Two waiters on one gate, one bounded tighter than the other: the tight one
// leaves the list and the loose one must still be reached by the settle. This
// is what the list's stable handles buy over the vector they replaced.
TEST_F(AsyncCompletionDeadlineTest, OtherWaitersSurviveAReleasedNeighbour) {
  scada::base::AsyncCompletion completion{executor()};
  auto tight = std::make_shared<std::optional<bool>>();
  auto loose = std::make_shared<std::optional<bool>>();

  boost::asio::co_spawn(
      context_,
      [completion, tight]() -> Awaitable<void> {
        tight->emplace(co_await completion.WaitFor(kTimeout));
      },
      boost::asio::detached);
  boost::asio::co_spawn(
      context_,
      [completion, loose]() -> Awaitable<void> {
        loose->emplace(co_await completion.WaitFor(20 * kTimeout));
      },
      boost::asio::detached);

  // Settle once the tight deadline has fired and well before the loose one.
  boost::asio::steady_timer settle{context_};
  settle.expires_after(4 * kTimeout);
  settle.async_wait(
      [completion](boost::system::error_code) { completion.TryComplete(); });

  const auto deadline = std::chrono::steady_clock::now() + kRunCap;
  while (!(*tight && *loose) && std::chrono::steady_clock::now() < deadline) {
    context_.restart();
    context_.run_for(2ms);
  }

  ASSERT_TRUE(*tight) << "the tight waiter never returned";
  ASSERT_TRUE(*loose) << "the loose waiter never returned";
  EXPECT_FALSE(**tight);
  EXPECT_TRUE(**loose);
}
