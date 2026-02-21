#include "scada/status.h"

#include <gtest/gtest.h>

// ToCString

TEST(StatusTest, ToCStringGood) {
  EXPECT_STREQ("Good", ToCString(scada::StatusCode::Good));
}

TEST(StatusTest, ToCStringBad) {
  EXPECT_STREQ("Bad", ToCString(scada::StatusCode::Bad));
}

TEST(StatusTest, ToCStringBadTimeout) {
  EXPECT_STREQ("Bad_Timeout", ToCString(scada::StatusCode::Bad_Timeout));
}

TEST(StatusTest, ToCStringUncertainStateWasNotChanged) {
  EXPECT_STREQ("Uncertain_StateWasNotChanged",
               ToCString(scada::StatusCode::Uncertain_StateWasNotChanged));
}

TEST(StatusTest, ToCStringUnknownGoodReturnsFallback) {
  auto unknown = static_cast<scada::StatusCode>(0x0FFF);
  EXPECT_STREQ("OK", ToCString(unknown));
}

TEST(StatusTest, ToCStringUnknownBadReturnsFallback) {
  auto unknown = static_cast<scada::StatusCode>(
      (static_cast<unsigned>(scada::StatusSeverity::Bad) << 14) | 0x3FFF);
  EXPECT_STREQ("Error", ToCString(unknown));
}

// ToString(StatusCode)

TEST(StatusTest, ToStringGood) {
  EXPECT_EQ("Good", ToString(scada::StatusCode::Good));
}

TEST(StatusTest, ToStringBadDisconnected) {
  EXPECT_EQ("Bad_Disconnected",
            ToString(scada::StatusCode::Bad_Disconnected));
}

// ToString16(StatusCode)

TEST(StatusTest, ToString16Good) {
  auto str = ToString16(scada::StatusCode::Good);
  EXPECT_FALSE(str.empty());
}

TEST(StatusTest, ToString16Bad) {
  auto str = ToString16(scada::StatusCode::Bad);
  EXPECT_FALSE(str.empty());
}

TEST(StatusTest, ToString16UnknownGoodReturnsFallback) {
  auto unknown = static_cast<scada::StatusCode>(0x0FFF);
  auto str = ToString16(unknown);
  EXPECT_FALSE(str.empty());
}

TEST(StatusTest, ToString16UnknownBadReturnsFallback) {
  auto unknown = static_cast<scada::StatusCode>(
      (static_cast<unsigned>(scada::StatusSeverity::Bad) << 14) | 0x3FFF);
  auto str = ToString16(unknown);
  EXPECT_FALSE(str.empty());
}

// ToString(Status) / ToString16(Status)

TEST(StatusTest, ToStringStatus) {
  scada::Status status(scada::StatusCode::Good);
  EXPECT_EQ("Good", ToString(status));
}

TEST(StatusTest, ToString16Status) {
  scada::Status status(scada::StatusCode::Bad_Timeout);
  auto str = ToString16(status);
  EXPECT_FALSE(str.empty());
}

// Status class

TEST(StatusTest, GoodStatusIsTrue) {
  scada::Status status(scada::StatusCode::Good);
  EXPECT_TRUE(static_cast<bool>(status));
  EXPECT_TRUE(status.good());
  EXPECT_FALSE(status.bad());
}

TEST(StatusTest, BadStatusIsFalse) {
  scada::Status status(scada::StatusCode::Bad);
  EXPECT_FALSE(static_cast<bool>(status));
  EXPECT_TRUE(status.bad());
  EXPECT_FALSE(status.good());
}

TEST(StatusTest, StatusCodeRoundTrip) {
  scada::Status status(scada::StatusCode::Bad_Timeout);
  EXPECT_EQ(scada::StatusCode::Bad_Timeout, status.code());
}

TEST(StatusTest, FromFullCode) {
  scada::Status original(scada::StatusCode::Bad_Disconnected);
  auto restored = scada::Status::FromFullCode(original.full_code());
  EXPECT_EQ(original, restored);
}

// IsGood / IsBad / IsUncertain

TEST(StatusTest, IsGoodForGoodCode) {
  EXPECT_TRUE(scada::IsGood(scada::StatusCode::Good));
  EXPECT_TRUE(scada::IsGood(scada::StatusCode::Good_Pending));
}

TEST(StatusTest, IsBadForBadCode) {
  EXPECT_TRUE(scada::IsBad(scada::StatusCode::Bad));
  EXPECT_TRUE(scada::IsBad(scada::StatusCode::Bad_Timeout));
}

TEST(StatusTest, IsUncertainForUncertainCode) {
  EXPECT_TRUE(scada::IsUncertain(scada::StatusCode::Uncertain));
  EXPECT_TRUE(
      scada::IsUncertain(scada::StatusCode::Uncertain_StateWasNotChanged));
}
