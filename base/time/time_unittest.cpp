#include "base/time/time.h"

#include <thread>

#include <gtest/gtest.h>

namespace base {

// TimeDelta tests ------------------------------------------------------------

TEST(TimeDeltaTest, FromSecondsInSeconds) {
  auto delta = TimeDelta::FromSeconds(5);
  EXPECT_EQ(5, delta.InSeconds());
  EXPECT_EQ(5000, delta.InMilliseconds());
  EXPECT_EQ(5000000, delta.InMicroseconds());
}

TEST(TimeDeltaTest, FromMilliseconds) {
  auto delta = TimeDelta::FromMilliseconds(1500);
  EXPECT_EQ(1, delta.InSeconds());
  EXPECT_EQ(1500, delta.InMilliseconds());
}

TEST(TimeDeltaTest, FromMicroseconds) {
  auto delta = TimeDelta::FromMicroseconds(2500000);
  EXPECT_EQ(2, delta.InSeconds());
  EXPECT_EQ(2500, delta.InMilliseconds());
}

TEST(TimeDeltaTest, FromNanoseconds) {
  auto delta = TimeDelta::FromNanoseconds(3000000000);
  EXPECT_EQ(3, delta.InSeconds());
}

TEST(TimeDeltaTest, FromHours) {
  auto delta = TimeDelta::FromHours(2);
  EXPECT_EQ(7200, delta.InSeconds());
  EXPECT_EQ(120, delta.InMinutes());
}

TEST(TimeDeltaTest, FromDays) {
  auto delta = TimeDelta::FromDays(1);
  EXPECT_EQ(24, delta.InHours());
  EXPECT_EQ(86400, delta.InSeconds());
}

TEST(TimeDeltaTest, InSecondsF) {
  auto delta = TimeDelta::FromMilliseconds(1500);
  EXPECT_DOUBLE_EQ(1.5, delta.InSecondsF());
}

TEST(TimeDeltaTest, InMillisecondsF) {
  auto delta = TimeDelta::FromMicroseconds(1500);
  EXPECT_DOUBLE_EQ(1.5, delta.InMillisecondsF());
}

TEST(TimeDeltaTest, InMillisecondsRoundedUp) {
  EXPECT_EQ(2, TimeDelta::FromMicroseconds(1001).InMillisecondsRoundedUp());
  EXPECT_EQ(1, TimeDelta::FromMicroseconds(1000).InMillisecondsRoundedUp());
  EXPECT_EQ(1, TimeDelta::FromMicroseconds(999).InMillisecondsRoundedUp());
}

TEST(TimeDeltaTest, InNanoseconds) {
  EXPECT_EQ(5000, TimeDelta::FromMicroseconds(5).InNanoseconds());
}

TEST(TimeDeltaTest, MaxMin) {
  EXPECT_TRUE(TimeDelta::Max().is_max());
  EXPECT_TRUE(TimeDelta::Min().is_min());
  EXPECT_FALSE(TimeDelta::Max().is_zero());
  EXPECT_FALSE(TimeDelta::Min().is_zero());
}

TEST(TimeDeltaTest, IsZero) {
  EXPECT_TRUE(TimeDelta().is_zero());
  EXPECT_FALSE(TimeDelta::FromSeconds(1).is_zero());
}

TEST(TimeDeltaTest, Arithmetic) {
  auto a = TimeDelta::FromSeconds(3);
  auto b = TimeDelta::FromSeconds(2);
  EXPECT_EQ(5, (a + b).InSeconds());
  EXPECT_EQ(1, (a - b).InSeconds());
  EXPECT_EQ(6, (a * 2).InSeconds());
  EXPECT_EQ(1, (a / 3).InSeconds());
  EXPECT_EQ(-3, (-a).InSeconds());
}

TEST(TimeDeltaTest, Comparisons) {
  auto a = TimeDelta::FromSeconds(3);
  auto b = TimeDelta::FromSeconds(2);
  EXPECT_GT(a, b);
  EXPECT_LT(b, a);
  EXPECT_GE(a, a);
  EXPECT_LE(a, a);
  EXPECT_EQ(a, a);
  EXPECT_NE(a, b);
}

TEST(TimeDeltaTest, FromInternalValue) {
  auto delta = TimeDelta::FromInternalValue(123456);
  EXPECT_EQ(123456, delta.ToInternalValue());
}

TEST(TimeDeltaTest, OverflowSaturation) {
  auto big = TimeDelta::FromSeconds(std::numeric_limits<int64_t>::max() / 1000);
  auto result = big + big;
  EXPECT_TRUE(result.is_max());
}

TEST(TimeDeltaTest, FromSecondsD) {
  auto delta = TimeDelta::FromSecondsD(1.5);
  EXPECT_EQ(1500000, delta.InMicroseconds());
}

TEST(TimeDeltaTest, FromMillisecondsD) {
  auto delta = TimeDelta::FromMillisecondsD(2.5);
  EXPECT_EQ(2500, delta.InMicroseconds());
}

TEST(TimeDeltaTest, Magnitude) {
  auto negative = TimeDelta::FromSeconds(-5);
  EXPECT_EQ(5, negative.magnitude().InSeconds());
}

TEST(TimeDeltaTest, DivisionByDelta) {
  auto a = TimeDelta::FromSeconds(10);
  auto b = TimeDelta::FromSeconds(2);
  EXPECT_EQ(5, a / b);
}

TEST(TimeDeltaTest, Modulo) {
  auto a = TimeDelta::FromSeconds(7);
  auto b = TimeDelta::FromSeconds(3);
  EXPECT_EQ(1, (a % b).InSeconds());
}

// Time tests -----------------------------------------------------------------

TEST(TimeTest, NowIsNotNull) {
  auto now = Time::Now();
  EXPECT_FALSE(now.is_null());
}

TEST(TimeTest, UnixEpoch) {
  auto epoch = Time::UnixEpoch();
  EXPECT_FALSE(epoch.is_null());
  EXPECT_EQ(Time::kTimeTToMicrosecondsOffset, epoch.ToInternalValue());
}

TEST(TimeTest, DeltaSinceWindowsEpochRoundTrip) {
  auto now = Time::Now();
  auto delta = now.ToDeltaSinceWindowsEpoch();
  auto restored = Time::FromDeltaSinceWindowsEpoch(delta);
  EXPECT_EQ(now, restored);
}

TEST(TimeTest, FromInternalValue) {
  int64_t value = 13000000000000000LL;
  auto time = Time::FromInternalValue(value);
  EXPECT_EQ(value, time.ToInternalValue());
}

TEST(TimeTest, IsNullMaxMin) {
  EXPECT_TRUE(Time().is_null());
  EXPECT_TRUE(Time::Max().is_max());
  EXPECT_TRUE(Time::Min().is_min());
  EXPECT_FALSE(Time::Now().is_null());
  EXPECT_FALSE(Time::Now().is_max());
}

TEST(TimeTest, UTCExplodeRoundTrip) {
  auto now = Time::Now();
  Time::Exploded exploded;
  now.UTCExplode(&exploded);
  EXPECT_TRUE(exploded.HasValidValues());

  Time restored;
  EXPECT_TRUE(Time::FromUTCExploded(exploded, &restored));
  // Round-trip should be within 1ms (Exploded has millisecond precision).
  auto diff = (now - restored).magnitude();
  EXPECT_LT(diff, TimeDelta::FromMilliseconds(1));
}

TEST(TimeTest, LocalExplodeRoundTrip) {
  auto now = Time::Now();
  Time::Exploded exploded;
  now.LocalExplode(&exploded);
  EXPECT_TRUE(exploded.HasValidValues());

  Time restored;
  EXPECT_TRUE(Time::FromLocalExploded(exploded, &restored));
  auto diff = (now - restored).magnitude();
  EXPECT_LT(diff, TimeDelta::FromMilliseconds(1));
}

TEST(TimeTest, LocalMidnight) {
  auto now = Time::Now();
  auto midnight = now.LocalMidnight();
  EXPECT_LE(midnight, now);

  Time::Exploded exploded;
  midnight.LocalExplode(&exploded);
  EXPECT_EQ(0, exploded.hour);
  EXPECT_EQ(0, exploded.minute);
  EXPECT_EQ(0, exploded.second);
  EXPECT_EQ(0, exploded.millisecond);
}

TEST(TimeTest, FromString) {
  Time time;
  EXPECT_TRUE(Time::FromString("Tue, 15 Nov 1994 12:45:26 GMT", &time));
  EXPECT_FALSE(time.is_null());
}

TEST(TimeTest, FromUTCString) {
  Time time;
  EXPECT_TRUE(Time::FromUTCString("Tue, 15 Nov 1994 12:45:26 GMT", &time));
  EXPECT_FALSE(time.is_null());

  Time::Exploded exploded;
  time.UTCExplode(&exploded);
  EXPECT_EQ(1994, exploded.year);
  EXPECT_EQ(11, exploded.month);
  EXPECT_EQ(15, exploded.day_of_month);
  EXPECT_EQ(12, exploded.hour);
  EXPECT_EQ(45, exploded.minute);
  EXPECT_EQ(26, exploded.second);
}

TEST(TimeTest, FromStringInvalid) {
  Time time;
  EXPECT_FALSE(Time::FromString("", &time));
  EXPECT_FALSE(Time::FromString("not a time", &time));
}

TEST(TimeTest, KMicrosecondsPerSecond) {
  EXPECT_EQ(1000000, Time::kMicrosecondsPerSecond);
}

TEST(TimeTest, ArithmeticWithTimeDelta) {
  auto now = Time::Now();
  auto later = now + TimeDelta::FromSeconds(10);
  auto diff = later - now;
  EXPECT_EQ(10, diff.InSeconds());
}

TEST(TimeTest, Comparisons) {
  auto a = Time::Now();
  auto b = a + TimeDelta::FromSeconds(1);
  EXPECT_LT(a, b);
  EXPECT_GT(b, a);
  EXPECT_EQ(a, a);
  EXPECT_NE(a, b);
}

// TimeTicks tests ------------------------------------------------------------

TEST(TimeTicksTest, NowIsNotNull) {
  auto now = TimeTicks::Now();
  EXPECT_FALSE(now.is_null());
}

TEST(TimeTicksTest, MonotonicallyIncreasing) {
  auto a = TimeTicks::Now();
  // Small sleep to guarantee progression.
  std::this_thread::sleep_for(std::chrono::milliseconds(1));
  auto b = TimeTicks::Now();
  EXPECT_LT(a, b);
}

TEST(TimeTicksTest, ArithmeticWithTimeDelta) {
  auto now = TimeTicks::Now();
  auto later = now + TimeDelta::FromMicroseconds(100);
  EXPECT_EQ(100, (later - now).InMicroseconds());
}

TEST(TimeTicksTest, DefaultIsNull) {
  TimeTicks ticks;
  EXPECT_TRUE(ticks.is_null());
}

}  // namespace base
