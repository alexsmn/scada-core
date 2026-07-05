#include "metrics/trace_parent.h"

#include <gtest/gtest.h>

namespace {

constexpr std::string_view kValid =
    "00-0af7651916cd43dd8448eb211c80319c-b7ad6b7169203331-01";

TEST(TraceParentTest, ParsesValidTraceParent) {
  auto parsed = ParseTraceParent(kValid);
  ASSERT_TRUE(parsed);
  EXPECT_EQ(parsed->trace_id[0], 0x0a);
  EXPECT_EQ(parsed->trace_id[15], 0x9c);
  EXPECT_EQ(parsed->span_id[0], 0xb7);
  EXPECT_EQ(parsed->span_id[7], 0x31);
  EXPECT_EQ(parsed->flags, 0x01);
  EXPECT_TRUE(parsed->sampled());
}

TEST(TraceParentTest, FormatRoundTrips) {
  auto parsed = ParseTraceParent(kValid);
  ASSERT_TRUE(parsed);
  EXPECT_EQ(FormatTraceParent(*parsed), kValid);
}

TEST(TraceParentTest, ParsesUnsampledFlags) {
  auto parsed = ParseTraceParent(
      "00-0af7651916cd43dd8448eb211c80319c-b7ad6b7169203331-00");
  ASSERT_TRUE(parsed);
  EXPECT_FALSE(parsed->sampled());
}

TEST(TraceParentTest, RejectsMalformedInput) {
  // Empty / garbage / legacy UUID span ids.
  EXPECT_FALSE(IsTraceParent(""));
  EXPECT_FALSE(IsTraceParent("garbage"));
  EXPECT_FALSE(IsTraceParent("550e8400-e29b-41d4-a716-446655440000"));

  // Wrong length.
  EXPECT_FALSE(
      IsTraceParent("00-0af7651916cd43dd8448eb211c80319c-b7ad6b7169203331-0"));
  EXPECT_FALSE(IsTraceParent(
      "00-0af7651916cd43dd8448eb211c80319c-b7ad6b7169203331-011"));

  // Unsupported version.
  EXPECT_FALSE(
      IsTraceParent("01-0af7651916cd43dd8448eb211c80319c-b7ad6b7169203331-01"));

  // Wrong separators.
  EXPECT_FALSE(
      IsTraceParent("00_0af7651916cd43dd8448eb211c80319c-b7ad6b7169203331-01"));

  // Uppercase hex is invalid per the spec.
  EXPECT_FALSE(
      IsTraceParent("00-0AF7651916CD43DD8448EB211C80319C-B7AD6B7169203331-01"));

  // Non-hex digits.
  EXPECT_FALSE(
      IsTraceParent("00-0az7651916cd43dd8448eb211c80319c-b7ad6b7169203331-01"));

  // All-zero trace id / span id.
  EXPECT_FALSE(
      IsTraceParent("00-00000000000000000000000000000000-b7ad6b7169203331-01"));
  EXPECT_FALSE(
      IsTraceParent("00-0af7651916cd43dd8448eb211c80319c-0000000000000000-01"));
}

TEST(TraceParentTest, IsTraceParentAcceptsValid) {
  EXPECT_TRUE(IsTraceParent(kValid));
}

}  // namespace
