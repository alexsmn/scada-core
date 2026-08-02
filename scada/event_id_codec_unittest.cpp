#include "scada/event_id_codec.h"

#include <gtest/gtest.h>

namespace scada {
namespace {

TEST(EventIdCodecTest, RoundTripsBigEndian) {
  // 8-byte big-endian layout, frozen; mirrors opcua::EncodeEventIdByteString.
  // OPC UA Part 5 §6.4.2 BaseEventType,
  // https://reference.opcfoundation.org/Core/Part5/v105/docs/6.4.2
  const EventId event_id = 0x0123456789abcdefull;
  const ByteString bytes = EncodeEventIdByteString(event_id);
  ASSERT_EQ(bytes.size(), 8u);
  EXPECT_EQ(static_cast<unsigned char>(bytes[0]), 0x01u);
  EXPECT_EQ(static_cast<unsigned char>(bytes[7]), 0xefu);
  EXPECT_EQ(DecodeEventIdByteString(bytes), event_id);

  // Lexicographic ByteString order equals numeric order.
  EXPECT_LT(EncodeEventIdByteString(1), EncodeEventIdByteString(0x100));
}

TEST(EventIdCodecTest, RejectsWrongSizePayloads) {
  EXPECT_EQ(DecodeEventIdByteString(ByteString{}), std::nullopt);
  EXPECT_EQ(DecodeEventIdByteString(ByteString(7)), std::nullopt);
  EXPECT_EQ(DecodeEventIdByteString(ByteString(9)), std::nullopt);
}

}  // namespace
}  // namespace scada
