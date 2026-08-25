#include "base/test/asio_test_environment.h"

#include <boost/asio/post.hpp>
#include <boost/asio/steady_timer.hpp>
#include <chrono>
#include <gtest/gtest.h>

namespace {

using namespace std::chrono_literals;

// The point of DrainUntil/RunUntil over PumpFor is that a wait costs what the
// work costs rather than the span its author guessed, and that a wait which
// never succeeds says so instead of passing silently. Both are properties of
// the harness itself, so they are pinned here rather than in whichever suite
// happens to call them first.

TEST(AsioTestEnvironmentTest, DrainUntilRunsPostedWorkAndReportsSuccess) {
  AsioTestEnvironment env;

  bool ran = false;
  boost::asio::post(env.executor, [&] { ran = true; });

  EXPECT_TRUE(env.DrainUntil([&] { return ran; }));
  EXPECT_TRUE(ran);
}

TEST(AsioTestEnvironmentTest, DrainUntilDrainsWorkThePredicateSatisfierPosted) {
  AsioTestEnvironment env;

  bool first = false;
  bool second = false;
  boost::asio::post(env.executor, [&] {
    first = true;
    boost::asio::post(env.executor, [&] { second = true; });
  });

  ASSERT_TRUE(env.DrainUntil([&] { return first; }));
  // The extra drain after the predicate holds is what lets a caller assert on
  // state the satisfying handler went on to produce.
  EXPECT_TRUE(second);
}

TEST(AsioTestEnvironmentTest, DrainUntilReportsFailureRatherThanHanging) {
  AsioTestEnvironment env;

  EXPECT_FALSE(env.DrainUntil([] { return false; }, /*max_iterations=*/10));
}

TEST(AsioTestEnvironmentTest, RunUntilReturnsAsSoonAsThePredicateHolds) {
  AsioTestEnvironment env;

  // A real timer, so the completion arrives through the reactor -- the case
  // DrainUntil cannot serve, and the reason RunUntil blocks on each pass.
  boost::asio::steady_timer timer{env.io_context};
  bool fired = false;
  timer.expires_after(5ms);
  timer.async_wait([&](auto) { fired = true; });

  const auto start = std::chrono::steady_clock::now();
  ASSERT_TRUE(env.RunUntil([&] { return fired; }, 1ms));
  const auto elapsed = std::chrono::steady_clock::now() - start;

  // The budget here is 1000 passes of 1 ms, and the equivalent fixed PumpFor
  // in the suites this replaced was 100-200 ms. Anything near either means the
  // wait stopped early-exiting and every caller silently got slower again.
  EXPECT_LT(elapsed, 100ms);
}

TEST(AsioTestEnvironmentTest, RunUntilReportsFailureRatherThanHanging) {
  AsioTestEnvironment env;

  EXPECT_FALSE(env.RunUntil([] { return false; }, 1ms, /*max_iterations=*/10));
}

}  // namespace
