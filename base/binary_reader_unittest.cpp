#include "base/binary_reader.h"

#include <gtest/gtest.h>

#include <array>
#include <stdexcept>
#include <vector>

namespace {

TEST(BinaryReaderTest, ReadsFixedWidthValuesInNativeOrder) {
  const uint32_t value = 0x01020304;
  BinaryReader reader{
      std::span{reinterpret_cast<const uint8_t*>(&value), sizeof(value)}};

  EXPECT_EQ(reader.ReadU32(), value);
  EXPECT_TRUE(reader.end());
}

TEST(BinaryReaderTest, ReadsBigEndianValues) {
  const std::array<uint8_t, 6> data{0x12, 0x34, 0xAB, 0xCD, 0xEF, 0x01};
  BinaryReader reader{std::span{data}};

  EXPECT_EQ(reader.ReadU16Be(), 0x1234);
  EXPECT_EQ(reader.ReadU32Be(), 0xABCDEF01u);
  EXPECT_TRUE(reader.end());
}

TEST(BinaryReaderTest, TracksRemainingAndEnd) {
  const std::array<uint8_t, 3> data{1, 2, 3};
  BinaryReader reader{std::span{data}};

  EXPECT_FALSE(reader.empty());
  EXPECT_EQ(reader.remaining(), 3u);
  EXPECT_EQ(reader.ReadU8(), 1u);
  EXPECT_EQ(reader.remaining(), 2u);
  EXPECT_FALSE(reader.end());

  reader.Skip(2);
  EXPECT_EQ(reader.remaining(), 0u);
  EXPECT_TRUE(reader.end());
}

TEST(BinaryReaderTest, ReadBytesViewAndRemainingBytes) {
  const std::array<uint8_t, 4> data{10, 20, 30, 40};
  BinaryReader reader{std::span{data}};

  auto head = reader.ReadBytesView(2);
  ASSERT_EQ(head.size(), 2u);
  EXPECT_EQ(head[0], 10u);
  EXPECT_EQ(head[1], 20u);

  auto rest = reader.RemainingBytes();
  ASSERT_EQ(rest.size(), 2u);
  EXPECT_EQ(rest[0], 30u);
  EXPECT_EQ(rest[1], 40u);
  EXPECT_EQ(reader.remaining(), 2u);  // RemainingBytes() does not consume.
}

TEST(BinaryReaderTest, ReadBytesCopiesOut) {
  const std::array<uint8_t, 3> data{7, 8, 9};
  BinaryReader reader{std::span{data}};

  std::vector<uint8_t> out(3);
  reader.ReadBytes(out);
  EXPECT_EQ(out, (std::vector<uint8_t>{7, 8, 9}));
  EXPECT_TRUE(reader.end());
}

TEST(BinaryReaderTest, ThrowsOnOutOfBoundsRead) {
  const std::array<uint8_t, 1> data{1};
  BinaryReader reader{std::span{data}};

  EXPECT_THROW(reader.ReadU16(), std::runtime_error);
  EXPECT_THROW(reader.Skip(2), std::runtime_error);
  EXPECT_THROW(reader.ReadBytesView(2), std::runtime_error);

  EXPECT_EQ(reader.ReadU8(), 1u);  // Failed reads do not advance.
  EXPECT_THROW(reader.ReadU8(), std::runtime_error);
}

TEST(BinaryReaderTest, AcceptsCharSpans) {
  const std::string contents = "\x02Hi";
  BinaryReader reader{std::span<const char>{contents}};

  EXPECT_EQ(reader.ReadString8(), "Hi");
  EXPECT_TRUE(reader.end());
}

TEST(BinaryReaderTest, ReadString8ThrowsOnTruncatedData) {
  const std::array<uint8_t, 2> data{5, 'a'};  // Length says 5, only 1 byte.
  BinaryReader reader{std::span{data}};

  EXPECT_THROW(reader.ReadString8(), std::runtime_error);
}

}  // namespace
