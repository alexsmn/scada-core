#include "base/bit_mask_string.h"

#include <algorithm>
#include <array>
#include <string>
#include <string_view>

#include <gtest/gtest.h>

namespace base {
namespace {

constexpr std::array<std::string_view, 3> kLabels{"Select", "Parameter",
                                                  "Test"};

TEST(BitMaskToStringTest, NoBitsSetIsEmptyArray) {
  EXPECT_EQ(BitMaskToString(0, kLabels), "[]");
}

TEST(BitMaskToStringTest, SingleBit) {
  EXPECT_EQ(BitMaskToString(0b001, kLabels), R"(["Select"])");
  EXPECT_EQ(BitMaskToString(0b100, kLabels), R"(["Test"])");
}

TEST(BitMaskToStringTest, MultipleBitsAreCommaSeparated) {
  EXPECT_EQ(BitMaskToString(0b101, kLabels), R"(["Select","Test"])");
}

TEST(BitMaskToStringTest, BitsWithoutLabelsAreIgnored) {
  // Bit 3 has no label -> ignored; only labeled bits appear.
  EXPECT_EQ(BitMaskToString(0b1001, kLabels), R"(["Select"])");
}

TEST(BitMaskToStringTest, HighBitDoesNotOverflow) {
  // With enough labels to reach bit 31 and beyond, the high bit must be handled
  // without shifting past the width of `unsigned` (the pre-fix `1 << i` was UB
  // for i >= 31). Labels beyond the width of unsigned are simply never matched.
  std::array<std::string_view, 40> labels;
  for (std::string_view& label : labels)
    label = "b";
  // Only bit 31 set.
  const unsigned mask = 1u << 31;
  EXPECT_EQ(BitMaskToString(mask, labels), R"(["b"])");
  // Highest representable unsigned: every bit set -> 32 labels emitted.
  const std::string all = BitMaskToString(0xFFFFFFFFu, labels);
  EXPECT_EQ(std::count(all.begin(), all.end(), '"'), 64);  // 32 quoted labels
}

}  // namespace
}  // namespace base
