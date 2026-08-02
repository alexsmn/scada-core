#include "base/time_utils.h"

#include "base/test/test_time.h"

#include <gtest/gtest.h>

#include <chrono>

// SerializeToString(TimeDelta) / Deserialize(str, TimeDelta&)

TEST(TimeUtilsTimeDelta, SerializeZero) {
  auto str = SerializeToString(scada::Duration{});
  EXPECT_EQ("0:00:00", str);
}

TEST(TimeUtilsTimeDelta, SerializeHoursMinutesSeconds) {
  scada::Duration delta =
      std::chrono::hours{2} + std::chrono::minutes{30} + std::chrono::seconds{15};
  EXPECT_EQ("2:30:15", SerializeToString(delta));
}

TEST(TimeUtilsTimeDelta, SerializeLargeHours) {
  scada::Duration delta = std::chrono::hours{100} +
                                 std::chrono::minutes{5} +
                                 std::chrono::seconds{3};
  EXPECT_EQ("100:05:03", SerializeToString(delta));
}

TEST(TimeUtilsTimeDelta, DeserializeValid) {
  scada::Duration delta;
  ASSERT_TRUE(Deserialize("1:02:03", delta));
  EXPECT_EQ(std::chrono::hours{1} + std::chrono::minutes{2} +
                std::chrono::seconds{3},
            delta);
}

TEST(TimeUtilsTimeDelta, DeserializeZero) {
  scada::Duration delta;
  ASSERT_TRUE(Deserialize("0:00:00", delta));
  EXPECT_EQ(scada::Duration{}, delta);
}

TEST(TimeUtilsTimeDelta, DeserializeInvalidFormat) {
  scada::Duration delta;
  EXPECT_FALSE(Deserialize("", delta));
  EXPECT_FALSE(Deserialize("1:02", delta));
  EXPECT_FALSE(Deserialize("1:02:03:04", delta));
  EXPECT_FALSE(Deserialize("abc", delta));
}

TEST(TimeUtilsTimeDelta, DeserializeInvalidValues) {
  scada::Duration delta;
  EXPECT_FALSE(Deserialize("a:02:03", delta));
  EXPECT_FALSE(Deserialize("1:bb:03", delta));
  EXPECT_FALSE(Deserialize("1:02:cc", delta));
}

TEST(TimeUtilsTimeDelta, RoundTrip) {
  scada::Duration original = std::chrono::hours{5} +
                                    std::chrono::minutes{45} +
                                    std::chrono::seconds{30};
  auto str = SerializeToString(original);
  scada::Duration parsed;
  ASSERT_TRUE(Deserialize(str, parsed));
  EXPECT_EQ(original, parsed);
}

// SerializeToString(Time) / Deserialize(str, Time&)

TEST(TimeUtilsTime, SerializeAndDeserialize) {
  auto time = TestTimeFromString("Tue, 15 Nov 1994 12:45:26 GMT");
  auto str = SerializeToString(time);
  EXPECT_EQ("1994-11-15 12:45:26", str);

  scada::Time parsed;
  ASSERT_TRUE(Deserialize(str, parsed));
  // Compare at second granularity (serialization truncates to seconds when
  // milliseconds are zero).
  EXPECT_EQ(time, parsed);
}

TEST(TimeUtilsTime, SerializeWithMilliseconds) {
  auto time = TestTimeFromString("Tue, 15 Nov 1994 12:45:26 GMT");
  time += std::chrono::milliseconds{123};
  auto str = SerializeToString(time);
  EXPECT_EQ("1994-11-15 12:45:26.123", str);
}

TEST(TimeUtilsTime, DeserializeInvalid) {
  scada::Time time;
  EXPECT_FALSE(Deserialize("not-a-date", time));
}

// InMilliseconds / InSeconds / InMicroseconds free helpers

TEST(TimeUtilsInMilliseconds, Basic) {
  EXPECT_EQ(3000, InMilliseconds(std::chrono::seconds{3}));
}

TEST(TimeUtilsInMilliseconds, SubSecond) {
  EXPECT_EQ(500, InMilliseconds(std::chrono::milliseconds{500}));
}

TEST(TimeUtilsInSeconds, Basic) {
  EXPECT_EQ(120, InSeconds(std::chrono::minutes{2}));
}

TEST(TimeUtilsInMicroseconds, Basic) {
  EXPECT_EQ(1'500'000, InMicroseconds(std::chrono::milliseconds{1500}));
}

// TruncateTimeTo

TEST(TimeUtilsTruncate, TruncateToSecond) {
  auto time = TestTimeFromString("Tue, 15 Nov 1994 12:45:26 GMT");
  time += std::chrono::milliseconds{500};
  auto truncated = TruncateTimeTo(time, std::chrono::seconds{1});
  EXPECT_EQ(TestTimeFromString("Tue, 15 Nov 1994 12:45:26 GMT"), truncated);
}

TEST(TimeUtilsTruncate, TruncateToMinute) {
  auto time = TestTimeFromString("Tue, 15 Nov 1994 12:45:26 GMT");
  auto truncated = TruncateTimeTo(time, std::chrono::minutes{1});
  EXPECT_EQ(TestTimeFromString("Tue, 15 Nov 1994 12:45:00 GMT"), truncated);
}

TEST(TimeUtilsTruncate, TruncateToHour) {
  auto time = TestTimeFromString("Tue, 15 Nov 1994 12:45:26 GMT");
  auto truncated = TruncateTimeTo(time, std::chrono::hours{1});
  EXPECT_EQ(TestTimeFromString("Tue, 15 Nov 1994 12:00:00 GMT"), truncated);
}
