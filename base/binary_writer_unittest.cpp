#include "base/binary_writer.h"

#include "base/binary_reader.h"

#include <gtest/gtest.h>

#include <array>
#include <stdexcept>
#include <string>

namespace {

TEST(BinaryWriterTest, RoundTripsThroughBinaryReader) {
  std::array<uint8_t, 64> storage;
  BinaryWriter writer{storage};

  writer.WriteU8(0x7F);
  writer.WriteU16(0x1234);
  writer.WriteU32(0xDEADBEEF);
  writer.WriteU16Be(0xA1B2);
  writer.WriteU32Be(0xC3D4E5F6);
  writer.WriteString8("hello");

  BinaryReader reader{writer.written()};
  EXPECT_EQ(reader.ReadU8(), 0x7Fu);
  EXPECT_EQ(reader.ReadU16(), 0x1234u);
  EXPECT_EQ(reader.ReadU32(), 0xDEADBEEFu);
  EXPECT_EQ(reader.ReadU16Be(), 0xA1B2u);
  EXPECT_EQ(reader.ReadU32Be(), 0xC3D4E5F6u);
  EXPECT_EQ(reader.ReadString8(), "hello");
  EXPECT_TRUE(reader.end());
}

TEST(BinaryWriterTest, BigEndianWritesNetworkOrderBytes) {
  std::array<uint8_t, 2> storage;
  BinaryWriter writer{storage};

  writer.WriteU16Be(0x1234);
  EXPECT_EQ(storage[0], 0x12u);
  EXPECT_EQ(storage[1], 0x34u);
}

TEST(BinaryWriterTest, TracksSizeAndCapacity) {
  std::array<uint8_t, 4> storage;
  BinaryWriter writer{storage};

  EXPECT_EQ(writer.size(), 0u);
  EXPECT_EQ(writer.capacity_left(), 4u);

  writer.WriteU16(1);
  EXPECT_EQ(writer.size(), 2u);
  EXPECT_EQ(writer.capacity_left(), 2u);
  EXPECT_EQ(writer.written().size(), 2u);
}

TEST(BinaryWriterTest, AllocateBytesReturnsWritableSpan) {
  std::array<uint8_t, 4> storage{};
  BinaryWriter writer{storage};

  auto span = writer.AllocateBytes(3);
  ASSERT_EQ(span.size(), 3u);
  span[0] = 1;
  span[1] = 2;
  span[2] = 3;

  EXPECT_EQ(writer.size(), 3u);
  EXPECT_EQ(storage[0], 1u);
  EXPECT_EQ(storage[1], 2u);
  EXPECT_EQ(storage[2], 3u);
}

TEST(BinaryWriterTest, ThrowsOnCapacityOverflow) {
  std::array<uint8_t, 2> storage;
  BinaryWriter writer{storage};

  EXPECT_THROW(writer.WriteU32(1), std::runtime_error);
  EXPECT_EQ(writer.size(), 0u);  // Failed writes do not advance.

  writer.WriteU16(1);
  EXPECT_THROW(writer.WriteU8(1), std::runtime_error);
}

TEST(BinaryWriterTest, WriteString8RejectsLongStrings) {
  std::array<uint8_t, 512> storage;
  BinaryWriter writer{storage};

  writer.WriteString8(std::string(255, 'a'));  // Max length is fine.
  EXPECT_THROW(writer.WriteString8(std::string(256, 'b')), std::runtime_error);
}

TEST(BinaryWriterTest, AcceptsCharSpans) {
  std::array<char, 4> storage;
  BinaryWriter writer{std::span<char>{storage}};

  writer.WriteU16(0x0102);
  EXPECT_EQ(writer.size(), 2u);
}

}  // namespace
