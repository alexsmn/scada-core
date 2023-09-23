#include "base/interval_util.h"

#include <gmock/gmock.h>

namespace {

using IntInterval = Interval<int>;

}

TEST(DateTimeUtil, Empty) {
  std::vector<IntInterval> ranges;
  UnionIntervals(ranges, {10, 20});
  const std::vector<IntInterval> kExpectedRanges = {{10, 20}};
  EXPECT_EQ(ranges, kExpectedRanges);
}

TEST(DateTimeUtil, InsertFirst) {
  std::vector<IntInterval> ranges = {{100, 200}};
  UnionIntervals(ranges, {50, 80});
  const std::vector<IntInterval> kExpectedRanges = {{50, 80}, {100, 200}};
  EXPECT_EQ(ranges, kExpectedRanges);
}

TEST(DateTimeUtil, ExpandFirst) {
  std::vector<IntInterval> ranges = {{100, 200}};
  UnionIntervals(ranges, {50, 100});
  const std::vector<IntInterval> kExpectedRanges = {{50, 200}};
  EXPECT_EQ(ranges, kExpectedRanges);
}

TEST(DateTimeUtil, ExpandFirstWithOverlapping) {
  std::vector<IntInterval> ranges = {{100, 200}};
  UnionIntervals(ranges, {50, 200});
  const std::vector<IntInterval> kExpectedRanges = {{50, 200}};
  EXPECT_EQ(ranges, kExpectedRanges);
}

TEST(DateTimeUtil, ExpandFirstBothSides) {
  std::vector<IntInterval> ranges = {{100, 200}, {300, 400}};
  UnionIntervals(ranges, {50, 250});
  const std::vector<IntInterval> kExpectedRanges = {{50, 250}, {300, 400}};
  EXPECT_EQ(ranges, kExpectedRanges);
}

TEST(DateTimeUtil, InsertLast) {
  std::vector<IntInterval> ranges = {{100, 200}};
  UnionIntervals(ranges, {250, 300});
  const std::vector<IntInterval> kExpectedRanges = {{100, 200}, {250, 300}};
  EXPECT_EQ(ranges, kExpectedRanges);
}

TEST(DateTimeUtil, ExpandLast) {
  std::vector<IntInterval> ranges = {{100, 200}};
  UnionIntervals(ranges, {200, 300});
  const std::vector<IntInterval> kExpectedRanges = {{100, 300}};
  EXPECT_EQ(ranges, kExpectedRanges);
}

TEST(DateTimeUtil, ExpandLastWithOverlapping) {
  std::vector<IntInterval> ranges = {{100, 200}};
  UnionIntervals(ranges, {150, 300});
  const std::vector<IntInterval> kExpectedRanges = {{100, 300}};
  EXPECT_EQ(ranges, kExpectedRanges);
}

TEST(DateTimeUtil, ExpandLastBothSides) {
  std::vector<IntInterval> ranges = {{100, 200}, {300, 400}};
  UnionIntervals(ranges, {250, 450});
  const std::vector<IntInterval> kExpectedRanges = {{100, 200}, {250, 450}};
  EXPECT_EQ(ranges, kExpectedRanges);
}

TEST(DateTimeUtil, MergeTwo) {
  std::vector<IntInterval> ranges = {{100, 200}, {300, 400}};
  UnionIntervals(ranges, {150, 350});
  const std::vector<IntInterval> kExpectedRanges = {{100, 400}};
  EXPECT_EQ(ranges, kExpectedRanges);
}

TEST(DateTimeUtil, MergeTwoAndExpandBothSides) {
  std::vector<IntInterval> ranges = {{100, 200}, {300, 400}};
  UnionIntervals(ranges, {50, 450});
  const std::vector<IntInterval> kExpectedRanges = {{50, 450}};
  EXPECT_EQ(ranges, kExpectedRanges);
}

TEST(IntervalUtil, IntervalsContain) {
  std::vector<IntInterval> ranges = {{100, 200}, {300, 400}, {500, 600}};
  EXPECT_FALSE(IntervalsContain(ranges, {50, 100}));
  EXPECT_TRUE(IntervalsContain(ranges, {100, 150}));
  EXPECT_TRUE(IntervalsContain(ranges, {100, 200}));
  EXPECT_TRUE(IntervalsContain(ranges, {150, 200}));
  EXPECT_FALSE(IntervalsContain(ranges, {200, 250}));
  EXPECT_FALSE(IntervalsContain(ranges, {250, 300}));
  EXPECT_TRUE(IntervalsContain(ranges, {300, 400}));
  EXPECT_TRUE(IntervalsContain(ranges, {300, 350}));
  EXPECT_TRUE(IntervalsContain(ranges, {350, 400}));
  EXPECT_FALSE(IntervalsContain(ranges, {400, 450}));
  EXPECT_FALSE(IntervalsContain(ranges, {450, 500}));
  EXPECT_TRUE(IntervalsContain(ranges, {500, 550}));
  EXPECT_TRUE(IntervalsContain(ranges, {500, 600}));
  EXPECT_TRUE(IntervalsContain(ranges, {550, 600}));
  EXPECT_FALSE(IntervalsContain(ranges, {600, 650}));
}
