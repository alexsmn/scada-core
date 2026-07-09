#include "scada/status.h"
#include "scada/status_or.h"
#include "scada/test/status_matchers.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <expected>

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
  EXPECT_EQ("Bad_Disconnected", ToString(scada::StatusCode::Bad_Disconnected));
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

TEST(StatusTest, OkStatusReturnsGoodStatus) {
  EXPECT_EQ(scada::StatusCode::Good, scada::OkStatus().code());
  EXPECT_TRUE(scada::OkStatus());
}

TEST(StatusTest, BadStatusReturnsBadStatus) {
  EXPECT_EQ(scada::StatusCode::Bad, scada::BadStatus().code());
  EXPECT_FALSE(scada::BadStatus());
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

TEST(StatusTest, TestStatusMatchersAcceptStatusAndStatusOr) {
  EXPECT_THAT(scada::Status{scada::StatusCode::Bad_Disconnected},
              scada::test::StatusIs(scada::StatusCode::Bad_Disconnected));

  const scada::StatusOr<int> bad_result{scada::StatusCode::Bad_Timeout};
  EXPECT_THAT(bad_result,
              scada::test::StatusIs(scada::StatusCode::Bad_Timeout));

  const scada::StatusOr<int> good_result{42};
  EXPECT_THAT(good_result, scada::test::IsOkAndHolds(testing::Eq(42)));
}

TEST(StatusTest, TestStatusMacrosAcceptOkResults) {
  EXPECT_OK(scada::Status{scada::StatusCode::Good});

  const scada::StatusOr<int> result{7};
  ASSERT_OK(result);
  ASSERT_OK_AND_ASSIGN(auto value, result);
  EXPECT_EQ(value, 7);
}

// StatusOr std::expected interface

TEST(StatusTest, StatusOrHasValue) {
  const scada::StatusOr<int> ok_result{1};
  EXPECT_TRUE(ok_result.has_value());

  const scada::StatusOr<int> bad_result{scada::StatusCode::Bad_Timeout};
  EXPECT_FALSE(bad_result.has_value());
}

TEST(StatusTest, StatusOrErrorReturnsStatus) {
  const scada::StatusOr<int> bad_result{scada::StatusCode::Bad_Timeout};
  EXPECT_EQ(scada::StatusCode::Bad_Timeout, bad_result.error().code());
}

TEST(StatusTest, StatusOrValueOr) {
  EXPECT_EQ(7, scada::StatusOr<int>{7}.value_or(5));
  EXPECT_EQ(5, scada::StatusOr<int>{scada::StatusCode::Bad}.value_or(5));
}

TEST(StatusTest, StatusOrEquality) {
  EXPECT_EQ(scada::StatusOr<int>{7}, scada::StatusOr<int>{7});
  EXPECT_NE(scada::StatusOr<int>{7}, scada::StatusOr<int>{8});
  EXPECT_NE(scada::StatusOr<int>{7},
            scada::StatusOr<int>{scada::StatusCode::Bad});
}

TEST(StatusTest, StatusOrMonadicTransform) {
  const scada::StatusOr<int> ok_result{21};
  const scada::StatusOr<int> doubled =
      ok_result.transform([](int value) { return value * 2; });
  EXPECT_THAT(doubled, scada::test::IsOkAndHolds(testing::Eq(42)));
}

TEST(StatusTest, StatusOrMonadicAndThenPropagatesError) {
  const scada::StatusOr<int> bad_result{scada::StatusCode::Bad_Timeout};
  const scada::StatusOr<int> result = bad_result.and_then(
      [](int value) { return std::expected<int, scada::Status>{value}; });
  EXPECT_THAT(result, scada::test::StatusIs(scada::StatusCode::Bad_Timeout));
}

TEST(StatusTest, StatusOrFromExpectedWithValue) {
  const std::expected<int, scada::Status> expected{42};
  const scada::StatusOr<int> result{expected};
  EXPECT_THAT(result, scada::test::IsOkAndHolds(testing::Eq(42)));
}

TEST(StatusTest, StatusOrFromExpectedWithBadStatus) {
  const std::expected<int, scada::Status> expected{
      std::unexpect, scada::Status{scada::StatusCode::Bad_Timeout}};
  const scada::StatusOr<int> result{expected};
  EXPECT_THAT(result, scada::test::StatusIs(scada::StatusCode::Bad_Timeout));
}

#if GTEST_HAS_DEATH_TEST
TEST(StatusTest, StatusOrValuePanicsWithoutValue) {
  const scada::StatusOr<int> result{scada::StatusCode::Bad};

  EXPECT_DEATH(
      { (void)result.value(); },
      "Panic: StatusOr value access without a value\r?\n.*"
      "status_unittest.cpp");
}

TEST(StatusTest, StatusOrOkStatusWithoutValuePanics) {
  EXPECT_DEATH(
      {
        const scada::StatusOr<int> result{scada::StatusCode::Good};
        (void)result;
      },
      "Panic: StatusOr constructed without a value from an ok status.*"
      "\r?\n.*status_or.h");
}

TEST(StatusTest, StatusOrFromExpectedWithOkStatusPanics) {
  const std::expected<int, scada::Status> expected{
      std::unexpect, scada::Status{scada::StatusCode::Good}};

  EXPECT_DEATH(
      {
        const scada::StatusOr<int> result{expected};
        (void)result;
      },
      "Panic: StatusOr constructed without a value from an ok status");
}

TEST(StatusTest, StatusOrErrorPanicsWithValuePresent) {
  const scada::StatusOr<int> result{7};

  EXPECT_DEATH(
      { (void)result.error(); },
      "Panic: StatusOr error access with a value present\r?\n.*"
      "status_unittest.cpp");
}
#endif
