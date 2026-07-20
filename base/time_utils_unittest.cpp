#include "base/time_utils.h"

#include "base/test/test_time.h"

#include <gtest/gtest.h>

// SerializeToString(TimeDelta) / Deserialize(str, TimeDelta&)

TEST(TimeUtilsTimeDelta, SerializeZero) {
  auto str = SerializeToString(scada::base::TimeDelta());
  EXPECT_EQ("0:00:00", str);
}

TEST(TimeUtilsTimeDelta, SerializeHoursMinutesSeconds) {
  auto delta = scada::base::TimeDelta::FromHours(2) +
               scada::base::TimeDelta::FromMinutes(30) +
               scada::base::TimeDelta::FromSeconds(15);
  EXPECT_EQ("2:30:15", SerializeToString(delta));
}

TEST(TimeUtilsTimeDelta, SerializeLargeHours) {
  auto delta = scada::base::TimeDelta::FromHours(100) +
               scada::base::TimeDelta::FromMinutes(5) +
               scada::base::TimeDelta::FromSeconds(3);
  EXPECT_EQ("100:05:03", SerializeToString(delta));
}

TEST(TimeUtilsTimeDelta, DeserializeValid) {
  scada::base::TimeDelta delta;
  ASSERT_TRUE(Deserialize("1:02:03", delta));
  EXPECT_EQ(scada::base::TimeDelta::FromHours(1) +
                scada::base::TimeDelta::FromMinutes(2) +
                scada::base::TimeDelta::FromSeconds(3),
            delta);
}

TEST(TimeUtilsTimeDelta, DeserializeZero) {
  scada::base::TimeDelta delta;
  ASSERT_TRUE(Deserialize("0:00:00", delta));
  EXPECT_EQ(scada::base::TimeDelta(), delta);
}

TEST(TimeUtilsTimeDelta, DeserializeInvalidFormat) {
  scada::base::TimeDelta delta;
  EXPECT_FALSE(Deserialize("", delta));
  EXPECT_FALSE(Deserialize("1:02", delta));
  EXPECT_FALSE(Deserialize("1:02:03:04", delta));
  EXPECT_FALSE(Deserialize("abc", delta));
}

TEST(TimeUtilsTimeDelta, DeserializeInvalidValues) {
  scada::base::TimeDelta delta;
  EXPECT_FALSE(Deserialize("a:02:03", delta));
  EXPECT_FALSE(Deserialize("1:bb:03", delta));
  EXPECT_FALSE(Deserialize("1:02:cc", delta));
}

TEST(TimeUtilsTimeDelta, RoundTrip) {
  auto original = scada::base::TimeDelta::FromHours(5) +
                  scada::base::TimeDelta::FromMinutes(45) +
                  scada::base::TimeDelta::FromSeconds(30);
  auto str = SerializeToString(original);
  scada::base::TimeDelta parsed;
  ASSERT_TRUE(Deserialize(str, parsed));
  EXPECT_EQ(original, parsed);
}

// SerializeToString(Time) / Deserialize(str, Time&)

TEST(TimeUtilsTime, SerializeAndDeserialize) {
  auto time = TestTimeFromString("Tue, 15 Nov 1994 12:45:26 GMT");
  auto str = SerializeToString(time);
  EXPECT_EQ("1994-11-15 12:45:26", str);

  scada::base::Time parsed;
  ASSERT_TRUE(Deserialize(str, parsed));
  // Compare at second granularity (serialization truncates to seconds when
  // milliseconds are zero).
  EXPECT_EQ(time, parsed);
}

TEST(TimeUtilsTime, SerializeWithMilliseconds) {
  auto time = TestTimeFromString("Tue, 15 Nov 1994 12:45:26 GMT");
  time += scada::base::TimeDelta::FromMilliseconds(123);
  auto str = SerializeToString(time);
  EXPECT_EQ("1994-11-15 12:45:26.123", str);
}

TEST(TimeUtilsTime, DeserializeInvalid) {
  scada::base::Time time;
  EXPECT_FALSE(Deserialize("not-a-date", time));
}

// TimeDeltaFromSecondsF

TEST(TimeUtilsTimeDeltaFromSecondsF, WholeSeconds) {
  auto delta = TimeDeltaFromSecondsF(5.0);
  EXPECT_EQ(5, delta.InSeconds());
}

TEST(TimeUtilsTimeDeltaFromSecondsF, FractionalSeconds) {
  auto delta = TimeDeltaFromSecondsF(1.5);
  EXPECT_EQ(1500000, delta.InMicroseconds());
}

TEST(TimeUtilsTimeDeltaFromSecondsF, Zero) {
  auto delta = TimeDeltaFromSecondsF(0.0);
  EXPECT_EQ(0, delta.InMicroseconds());
}

// InMilliseconds / InSeconds

TEST(TimeUtilsInMilliseconds, Basic) {
  auto duration = std::chrono::seconds(3);
  EXPECT_EQ(3000, InMilliseconds(duration));
}

TEST(TimeUtilsInMilliseconds, SubSecond) {
  auto duration = std::chrono::milliseconds(500);
  EXPECT_EQ(500, InMilliseconds(duration));
}

TEST(TimeUtilsInSeconds, Basic) {
  auto duration = std::chrono::minutes(2);
  EXPECT_EQ(120, InSeconds(duration));
}

// AsChrono

TEST(TimeUtilsAsChrono, TimeDeltaToMilliseconds) {
  auto delta = scada::base::TimeDelta::FromMilliseconds(1500);
  auto chrono_ms = AsChrono<std::chrono::milliseconds>(delta);
  EXPECT_EQ(1500, chrono_ms.count());
}

TEST(TimeUtilsAsChrono, TimeDeltaToNanoseconds) {
  auto delta = scada::base::TimeDelta::FromMicroseconds(100);
  auto chrono_ns = AsChrono<std::chrono::nanoseconds>(delta);
  EXPECT_EQ(100000, chrono_ns.count());
}

TEST(TimeUtilsAsChrono, TimeToTimePoint) {
  auto base_time = scada::base::Time::UnixEpoch() +
                   scada::base::TimeDelta::FromSeconds(1000);
  auto chrono_tp = AsChrono(base_time);
  auto since_epoch =
      std::chrono::duration_cast<std::chrono::seconds>(
          chrono_tp.time_since_epoch())
          .count();
  EXPECT_EQ(1000, since_epoch);
}

// TruncateTimeTo

TEST(TimeUtilsTruncate, TruncateToSecond) {
  auto time = TestTimeFromString("Tue, 15 Nov 1994 12:45:26 GMT");
  time += scada::base::TimeDelta::FromMilliseconds(500);
  auto truncated = TruncateTimeTo(time, scada::base::TimeDelta::FromSeconds(1));
  EXPECT_EQ(TestTimeFromString("Tue, 15 Nov 1994 12:45:26 GMT"), truncated);
}

TEST(TimeUtilsTruncate, TruncateToMinute) {
  auto time = TestTimeFromString("Tue, 15 Nov 1994 12:45:26 GMT");
  auto truncated = TruncateTimeTo(time, scada::base::TimeDelta::FromMinutes(1));
  EXPECT_EQ(TestTimeFromString("Tue, 15 Nov 1994 12:45:00 GMT"), truncated);
}

TEST(TimeUtilsTruncate, TruncateToHour) {
  auto time = TestTimeFromString("Tue, 15 Nov 1994 12:45:26 GMT");
  auto truncated = TruncateTimeTo(time, scada::base::TimeDelta::FromHours(1));
  EXPECT_EQ(TestTimeFromString("Tue, 15 Nov 1994 12:00:00 GMT"), truncated);
}
