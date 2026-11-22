#include "remote/login_throttle.h"

#include <gtest/gtest.h>

#include <chrono>
#include <string>
#include <vector>

using namespace std::chrono_literals;

namespace {

class LoginThrottleTest : public testing::Test {
 protected:
  // A policy small enough to walk through in a test, with the default shape:
  // two free failures, then 1 s doubling to an 8 s cap, and an address
  // lockout at five failures.
  static LoginThrottlePolicy TestPolicy() {
    return {.free_failures = 2,
            .base_delay = 1s,
            .max_delay = 8s,
            .lockout_failures = 5,
            .lockout_window = 1min,
            .forget_after = 10min,
            .max_tracked_keys = 64};
  }

  LoginThrottle MakeThrottle(LoginThrottlePolicy policy = TestPolicy()) {
    return LoginThrottle{std::move(policy), [this] { return now_; },
                         /*sleep=*/nullptr};
  }

  // Begins `count` attempts by `keys` and returns the delay of each.
  static std::vector<Clock::duration> Attempts(LoginThrottle& throttle,
                                               const LoginThrottle::Keys& keys,
                                               int count) {
    std::vector<Clock::duration> delays;
    for (int i = 0; i < count; ++i) {
      delays.push_back(throttle.BeginAttempt(keys).delay);
    }
    return delays;
  }

  TimePoint now_ = TimePoint{} + 1h;

  inline static const LoginThrottle::Keys kRootFromA{.user_name = "root",
                                                     .address = "10.0.0.1"};
};

TEST_F(LoginThrottleTest, DelayGrowsExponentiallyAfterTheFreeFailures) {
  LoginThrottle throttle = MakeThrottle();

  EXPECT_EQ(Attempts(throttle, kRootFromA, 5),
            (std::vector<Clock::duration>{0s, 0s, 1s, 2s, 4s}));
}

TEST_F(LoginThrottleTest, DelayIsCappedAtMaxDelay) {
  LoginThrottlePolicy policy = TestPolicy();
  policy.lockout_failures = 0;  // only the delay is under test
  LoginThrottle throttle = MakeThrottle(policy);

  EXPECT_EQ(Attempts(throttle, kRootFromA, 8),
            (std::vector<Clock::duration>{0s, 0s, 1s, 2s, 4s, 8s, 8s, 8s}));
}

TEST_F(LoginThrottleTest, CapHoldsAfterManyFailures) {
  LoginThrottlePolicy policy = TestPolicy();
  policy.lockout_failures = 0;
  policy.max_delay = 30s;
  LoginThrottle throttle = MakeThrottle(policy);

  std::ignore = Attempts(throttle, kRootFromA, 200);
  EXPECT_EQ(throttle.BeginAttempt(kRootFromA).delay, 30s);
}

TEST_F(LoginThrottleTest, SuccessResetsTheUserAndTheAddress) {
  LoginThrottle throttle = MakeThrottle();
  std::ignore = Attempts(throttle, kRootFromA, 4);

  throttle.RecordSuccess(kRootFromA);

  EXPECT_EQ(Attempts(throttle, kRootFromA, 3),
            (std::vector<Clock::duration>{0s, 0s, 1s}));
  // Neither key is delayed when presented with a fresh partner either.
  throttle.RecordSuccess(kRootFromA);
  EXPECT_EQ(
      throttle.BeginAttempt({.user_name = "root", .address = "10.9.9.9"}).delay,
      0s);
  EXPECT_EQ(
      throttle.BeginAttempt({.user_name = "bob", .address = "10.0.0.1"}).delay,
      0s);
}

TEST_F(LoginThrottleTest, AddressesAndUsersAreIsolated) {
  LoginThrottle throttle = MakeThrottle();
  std::ignore = Attempts(throttle, kRootFromA, 4);

  // Another user from another address is unaffected.
  EXPECT_EQ(Attempts(throttle, {.user_name = "bob", .address = "10.0.0.2"}, 3),
            (std::vector<Clock::duration>{0s, 0s, 1s}));
}

TEST_F(LoginThrottleTest, OneUserIsDelayedFromEveryAddress) {
  LoginThrottle throttle = MakeThrottle();

  // A guesser spreading attempts on one account over many addresses is still
  // delayed, because the user name is a key of its own.
  std::vector<Clock::duration> delays;
  for (int i = 0; i < 4; ++i) {
    delays.push_back(
        throttle
            .BeginAttempt(
                {.user_name = "root", .address = "10.1.0." + std::to_string(i)})
            .delay);
  }
  EXPECT_EQ(delays, (std::vector<Clock::duration>{0s, 0s, 1s, 2s}));
}

TEST_F(LoginThrottleTest, OneAddressIsDelayedForEveryUser) {
  LoginThrottle throttle = MakeThrottle();

  std::vector<Clock::duration> delays;
  for (int i = 0; i < 4; ++i) {
    delays.push_back(throttle
                         .BeginAttempt({.user_name = "user" + std::to_string(i),
                                        .address = "10.0.0.1"})
                         .delay);
  }
  EXPECT_EQ(delays, (std::vector<Clock::duration>{0s, 0s, 1s, 2s}));
}

TEST_F(LoginThrottleTest, AddressIsLockedOutForTheWindowThenGetsOneTry) {
  LoginThrottle throttle = MakeThrottle();

  for (int i = 0; i < 5; ++i) {
    EXPECT_FALSE(throttle.BeginAttempt(kRootFromA).locked_out) << i;
  }
  // The fifth failure locked the address; any user from it is refused.
  EXPECT_TRUE(throttle.BeginAttempt(kRootFromA).locked_out);
  EXPECT_TRUE(throttle.BeginAttempt({.user_name = "bob", .address = "10.0.0.1"})
                  .locked_out);
  // Another address is not.
  EXPECT_FALSE(
      throttle.BeginAttempt({.user_name = "bob", .address = "10.0.0.2"})
          .locked_out);

  now_ += 1min;
  EXPECT_FALSE(throttle.BeginAttempt(kRootFromA).locked_out);
  // That try failed (no RecordSuccess), so the address re-locks at once.
  EXPECT_TRUE(throttle.BeginAttempt(kRootFromA).locked_out);
}

TEST_F(LoginThrottleTest, LockedOutAttemptsDoNotExtendTheLockout) {
  LoginThrottle throttle = MakeThrottle();
  std::ignore = Attempts(throttle, kRootFromA, 5);

  for (int i = 0; i < 100; ++i) {
    now_ += 500ms;
    std::ignore = throttle.BeginAttempt(kRootFromA);
  }
  // 50 s into a 60 s window; at 60 s the window has passed regardless of the
  // hammering in between.
  now_ += 10s;
  EXPECT_FALSE(throttle.BeginAttempt(kRootFromA).locked_out);
}

TEST_F(LoginThrottleTest, AccountsAreNeverLockedOut) {
  LoginThrottle throttle = MakeThrottle();

  // Root guessed at from more addresses than the lockout threshold: each
  // attempt is delayed, none is refused, and the delay stays at the cap.
  for (int i = 0; i < 50; ++i) {
    LoginThrottle::Admission admission = throttle.BeginAttempt(
        {.user_name = "root", .address = "10.2.0." + std::to_string(i)});
    EXPECT_FALSE(admission.locked_out) << i;
    EXPECT_LE(admission.delay, 8s) << i;
  }

  // With no address known, nothing can be locked out at all.
  for (int i = 0; i < 50; ++i) {
    EXPECT_FALSE(throttle.BeginAttempt({.user_name = "root"}).locked_out);
  }
}

TEST_F(LoginThrottleTest, IdleKeysAreForgotten) {
  LoginThrottle throttle = MakeThrottle();
  std::ignore = Attempts(throttle, kRootFromA, 4);

  now_ += 10min;

  EXPECT_EQ(throttle.BeginAttempt(kRootFromA).delay, 0s);
}

TEST_F(LoginThrottleTest, TrackedKeysAreBounded) {
  LoginThrottlePolicy policy = TestPolicy();
  policy.max_tracked_keys = 8;
  LoginThrottle throttle = MakeThrottle(policy);

  for (int i = 0; i < 100; ++i) {
    now_ += 1s;
    std::ignore =
        throttle.BeginAttempt({.user_name = "u" + std::to_string(i),
                               .address = "10.3.0." + std::to_string(i)});
    EXPECT_LE(throttle.tracked_key_count(), 8u);
  }
}

TEST_F(LoginThrottleTest, EvictionPrefersKeysThatAreNotLockedOut) {
  LoginThrottlePolicy policy = TestPolicy();
  policy.max_tracked_keys = 4;
  LoginThrottle throttle = MakeThrottle(policy);
  const LoginThrottle::Keys locked{.address = "10.0.0.9"};
  std::ignore = Attempts(throttle, locked, 5);

  for (int i = 0; i < 20; ++i) {
    now_ += 1s;
    std::ignore =
        throttle.BeginAttempt({.address = "10.4.0." + std::to_string(i)});
  }

  EXPECT_TRUE(throttle.BeginAttempt(locked).locked_out);
}

TEST(LoginThrottlePeerAddressTest, StripsThePort) {
  EXPECT_EQ(LoginThrottlePeerAddress("192.168.1.5:51234"), "192.168.1.5");
  EXPECT_EQ(LoginThrottlePeerAddress("[::1]:2000"), "::1");
  EXPECT_EQ(LoginThrottlePeerAddress("[fe80::1%en0]:2000"), "fe80::1%en0");
}

TEST(LoginThrottlePeerAddressTest, LeavesAPortlessPeerAlone) {
  EXPECT_EQ(LoginThrottlePeerAddress("192.168.1.5"), "192.168.1.5");
  EXPECT_EQ(LoginThrottlePeerAddress("fe80::1"), "fe80::1");
  EXPECT_EQ(LoginThrottlePeerAddress(""), "");
}

}  // namespace
