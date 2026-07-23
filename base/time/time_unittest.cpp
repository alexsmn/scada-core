#include "base/time/time.h"

#include "base/time/calendar.h"

#include <gtest/gtest.h>

#include <chrono>

namespace scada::base {
namespace {

// Time / TimeDelta are std::chrono aliases, so their arithmetic, factories and
// comparisons are exercised by the standard library. These tests cover the
// scada-owned behavior layered on top: the null/sentinel vocabulary and the
// calendar / string conversions.

// Null & sentinels -----------------------------------------------------------

TEST(TimeTest, NullSentinel) {
  // The default-constructed Time is the Unix epoch, NOT null.
  EXPECT_FALSE(IsNull(Time{}));
  EXPECT_TRUE(IsNull(kNullTime));
  // kNullTime is the 1601 Windows epoch: kTimeTToMicrosecondsOffset µs before
  // the Unix epoch.
  EXPECT_EQ((Time{} - kNullTime).count(), kTimeTToMicrosecondsOffset);
}

TEST(TimeTest, RangeSentinels) {
  EXPECT_EQ(kMaxTime, Time::max());
  EXPECT_EQ(kMinTime, Time::min());
  EXPECT_LT(kMinTime, kNullTime);
  EXPECT_LT(NowUtc(), kMaxTime);
}

TEST(TimeTest, NowIsNotNull) {
  EXPECT_FALSE(IsNull(NowUtc()));
}

// Calendar conversions -------------------------------------------------------

TEST(TimeTest, UtcExplodeRoundTrip) {
  const Time now = NowUtc();
  const Exploded exploded = UtcExplode(now);
  EXPECT_TRUE(exploded.HasValidValues());

  const std::optional<Time> restored = FromUtcExploded(exploded);
  ASSERT_TRUE(restored.has_value());
  // Exploded carries millisecond precision, so round-trip is within 1ms.
  EXPECT_LT(std::chrono::abs(now - *restored), std::chrono::milliseconds{1});
}

TEST(TimeTest, LocalExplodeRoundTrip) {
  const Time now = NowUtc();
  const Exploded exploded = LocalExplode(now);
  EXPECT_TRUE(exploded.HasValidValues());

  const std::optional<Time> restored = FromLocalExploded(exploded);
  ASSERT_TRUE(restored.has_value());
  EXPECT_LT(std::chrono::abs(now - *restored), std::chrono::milliseconds{1});
}

TEST(TimeTest, UtcExplodePinsKnownInstant) {
  // 1994-11-15 12:45:26.000 UTC.
  const std::optional<Time> time =
      TimeFromUtcString("Tue, 15 Nov 1994 12:45:26 GMT");
  ASSERT_TRUE(time.has_value());
  const Exploded e = UtcExplode(*time);
  EXPECT_EQ(1994, e.year);
  EXPECT_EQ(11, e.month);
  EXPECT_EQ(15, e.day_of_month);
  EXPECT_EQ(12, e.hour);
  EXPECT_EQ(45, e.minute);
  EXPECT_EQ(26, e.second);
}

TEST(TimeTest, FromUtcExplodedRejectsInvalid) {
  Exploded invalid = {};
  invalid.year = 2020;
  invalid.month = 2;
  invalid.day_of_month = 30;  // Feb 30 does not exist.
  invalid.hour = 12;
  EXPECT_FALSE(FromUtcExploded(invalid).has_value());
}

TEST(TimeTest, LocalMidnight) {
  const Time now = NowUtc();
  const Time midnight = LocalMidnight(now);
  EXPECT_LE(midnight, now);

  const Exploded e = LocalExplode(midnight);
  EXPECT_EQ(0, e.hour);
  EXPECT_EQ(0, e.minute);
  EXPECT_EQ(0, e.second);
  EXPECT_EQ(0, e.millisecond);
}

// String parsing -------------------------------------------------------------

TEST(TimeTest, TimeFromUtcStringFormats) {
  EXPECT_TRUE(TimeFromUtcString("2021-11-07T12:41:21").has_value());
  EXPECT_TRUE(TimeFromUtcString("2004-11-15 10:00:00").has_value());
  EXPECT_TRUE(TimeFromUtcString("15 Nov 2004 10:00:00 UTC").has_value());
  EXPECT_TRUE(TimeFromUtcString("Tue, 15 Nov 1994 12:45:26 GMT").has_value());
}

TEST(TimeTest, TimeFromUtcStringMilliseconds) {
  const std::optional<Time> time = TimeFromUtcString("1994-11-15 12:45:26.123");
  ASSERT_TRUE(time.has_value());
  EXPECT_EQ(123, UtcExplode(*time).millisecond);
}

TEST(TimeTest, TimeFromStringInvalid) {
  EXPECT_FALSE(TimeFromString("", /*is_local=*/true).has_value());
  EXPECT_FALSE(TimeFromString("not a time", /*is_local=*/true).has_value());
}

TEST(TimeTest, TimeFromStringLocalVsUtc) {
  // Both forms parse; the local form (no tz suffix) is interpreted in local
  // time, the UTC form as UTC. Just verify both succeed.
  EXPECT_TRUE(TimeFromUtcString("15 Nov 2004 10:00:00").has_value());
  EXPECT_TRUE(TimeFromString("15 Nov 2004 10:00:00", /*is_local=*/true)
                  .has_value());
}

}  // namespace
}  // namespace scada::base
