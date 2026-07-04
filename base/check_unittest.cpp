#include "base/check.h"

#include <gtest/gtest.h>

#include <string>
#include <thread>

#include "base/threading/thread_checker.h"

namespace base {
namespace {

TEST(CheckTest, PassingCheckIsNoOp) {
  Check(true);
  Check(true, "message");
  Check(true, [] { return "lazy message"; });
}

TEST(CheckDeathTest, FailingCheckPanics) {
  EXPECT_DEATH(Check(false), "Check failed");
}

TEST(CheckDeathTest, FailingCheckReportsMessage) {
  EXPECT_DEATH(Check(false, "custom invariant text"), "custom invariant text");
}

TEST(CheckDeathTest, LazyMessageBuiltOnFailure) {
  EXPECT_DEATH(Check(false, [] { return std::string("formatted 42"); }),
               "formatted 42");
}

TEST(CheckTest, LazyMessageNotBuiltOnSuccess) {
  bool invoked = false;
  Check(true, [&] {
    invoked = true;
    return "unused";
  });
  EXPECT_FALSE(invoked);
}

TEST(CheckDeathTest, NotReachedPanics) {
  EXPECT_DEATH(NotReached(), "NotReached");
}

TEST(CheckTest, ThreadCheckerPassesOnBoundThread) {
  ThreadChecker checker;
  EXPECT_TRUE(checker.CalledOnValidThread());
  checker.CheckCalledOnValidThread();
}

TEST(CheckTest, ThreadCheckerRebindsAfterDetach) {
  ThreadChecker checker;
  checker.DetachFromThread();
  EXPECT_TRUE(checker.CalledOnValidThread());
  checker.CheckCalledOnValidThread();
}

TEST(CheckDeathTest, ThreadCheckerPanicsOnWrongThread) {
  ThreadChecker checker;
  // The checked statement runs in the death-test child process; the panic
  // happens on a thread other than the one the checker is bound to.
  EXPECT_DEATH(
      {
        std::thread other{[&] { checker.CheckCalledOnValidThread(); }};
        other.join();
      },
      "Called on invalid thread");
}

}  // namespace
}  // namespace base
