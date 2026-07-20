#include "base/any_executor_timer.h"

#include <boost/asio/io_context.hpp>
#include <gtest/gtest.h>

#include <chrono>

namespace {

using namespace std::chrono_literals;

// A period no test is willing to wait out, so a timer that is not actually
// cancelled shows up as an io_context that never drains.
constexpr auto kNeverInTestTime = 1h;

// Long enough for a cancelled timer's completion to be delivered, short enough
// to keep a regressed test fast.
constexpr auto kDrainBudget = 1s;

// Regression: Stop() used to drop the Core while leaving the armed asio timer
// alive inside PostDelayedTask's own shared_ptr. The io_context then reported
// outstanding work -- with nothing runnable -- until the full period elapsed,
// which stalled ServerProcess's shutdown drain.
TEST(AnyExecutorTimer, StopCancelsArmedWaitSoContextDrains) {
  boost::asio::io_context io_context;

  bool fired = false;
  AnyExecutorTimer timer{io_context.get_executor()};
  timer.StartRepeating(kNeverInTestTime, [&] { fired = true; });
  timer.Stop();

  io_context.run_for(kDrainBudget);

  EXPECT_TRUE(io_context.stopped());
  EXPECT_FALSE(fired);
}

// Same contract for the implicit stop: letting the timer go out of scope must
// release the io_context, not pin it for the remainder of the period.
TEST(AnyExecutorTimer, DestructionCancelsArmedWaitSoContextDrains) {
  boost::asio::io_context io_context;

  {
    AnyExecutorTimer timer{io_context.get_executor()};
    timer.StartRepeating(kNeverInTestTime, [] {});
  }

  io_context.run_for(kDrainBudget);

  EXPECT_TRUE(io_context.stopped());
}

TEST(AnyExecutorTimer, StartOneCancelsArmedWaitSoContextDrains) {
  boost::asio::io_context io_context;

  AnyExecutorTimer timer{io_context.get_executor()};
  timer.StartOne(kNeverInTestTime, [] {});
  timer.Stop();

  io_context.run_for(kDrainBudget);

  EXPECT_TRUE(io_context.stopped());
}

// Guards the other direction: cancellation must not have cost the timer its
// actual job of firing periodically.
TEST(AnyExecutorTimer, RepeatingTimerKeepsFiringUntilStopped) {
  boost::asio::io_context io_context;

  int count = 0;
  AnyExecutorTimer timer{io_context.get_executor()};
  timer.StartRepeating(1ms, [&] {
    if (++count == 3)
      timer.Stop();
  });

  io_context.run_for(10s);

  EXPECT_EQ(count, 3);
  EXPECT_TRUE(io_context.stopped());
}

TEST(AnyExecutorTimer, StartOneFiresExactlyOnce) {
  boost::asio::io_context io_context;

  int count = 0;
  AnyExecutorTimer timer{io_context.get_executor()};
  timer.StartOne(1ms, [&] { ++count; });

  io_context.run_for(10s);

  EXPECT_EQ(count, 1);
  EXPECT_TRUE(io_context.stopped());
}

}  // namespace
