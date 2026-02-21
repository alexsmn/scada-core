#include "scada/date_time.h"

#include "base/format_time.h"

#include <gtest/gtest.h>

namespace {

scada::DateTime MakeUTCTime(int year, int month, int day, int hour, int minute,
                            int second, int millisecond = 0) {
  base::Time::Exploded exploded = {};
  exploded.year = year;
  exploded.month = month;
  exploded.day_of_month = day;
  exploded.hour = hour;
  exploded.minute = minute;
  exploded.second = second;
  exploded.millisecond = millisecond;
  base::Time time;
  EXPECT_TRUE(base::Time::FromUTCExploded(exploded, &time));
  return time;
}

}  // namespace

TEST(DateTimeTest, ToStringFormat) {
  auto time = MakeUTCTime(1994, 11, 15, 12, 45, 26);
  auto str = FormatTime(time, TIME_FORMAT_DEFAULT | TIME_FORMAT_UTC);
  EXPECT_EQ("15.11.1994 12:45:26.000", str);
}

TEST(DateTimeTest, ToStringFormatWithMilliseconds) {
  auto time = MakeUTCTime(2024, 6, 15, 10, 30, 45, 123);
  auto str = FormatTime(time, TIME_FORMAT_DEFAULT | TIME_FORMAT_UTC);
  EXPECT_EQ("15.06.2024 10:30:45.123", str);
}

TEST(DateTimeTest, ToStringNotEmpty) {
  auto time = MakeUTCTime(2024, 1, 1, 0, 0, 0);
  EXPECT_FALSE(ToString(time).empty());
}

TEST(DateTimeTest, ToStringNullIsEmpty) {
  EXPECT_TRUE(ToString(base::Time()).empty());
}

TEST(DateTimeTest, ToString16NotEmpty) {
  auto time = MakeUTCTime(2024, 1, 1, 0, 0, 0);
  EXPECT_FALSE(ToString16(time).empty());
}

TEST(DateTimeTest, ToString16NullIsEmpty) {
  EXPECT_TRUE(ToString16(base::Time()).empty());
}

TEST(DateTimeTest, ToString16MatchesToString) {
  auto time = MakeUTCTime(1994, 11, 15, 12, 45, 26);
  auto narrow = ToString(time);
  auto wide = ToString16(time);
  EXPECT_EQ(narrow.size(), wide.size());
}

TEST(DateTimeTest, ToStringEpoch) {
  auto time = MakeUTCTime(1970, 1, 1, 0, 0, 0);
  auto str = FormatTime(time, TIME_FORMAT_DEFAULT | TIME_FORMAT_UTC);
  EXPECT_EQ("01.01.1970 00:00:00.000", str);
}
