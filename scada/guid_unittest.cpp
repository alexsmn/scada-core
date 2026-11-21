#include "scada/guid.h"

#include <string>

#include <gmock/gmock.h>

namespace scada {
namespace {

constexpr std::string_view kText = "C496578A-0DFE-4B8F-870A-745238C6AEAE";

TEST(GuidTest, TextRoundTrip) {
  const std::optional<Guid> guid = Guid::FromString(kText);
  ASSERT_TRUE(guid.has_value());
  EXPECT_EQ(guid->data1, 0xC496578Au);
  EXPECT_EQ(guid->data2, 0x0DFEu);
  EXPECT_EQ(guid->data3, 0x4B8Fu);
  EXPECT_EQ(guid->data4[0], 0x87u);
  EXPECT_EQ(guid->data4[7], 0xAEu);
  EXPECT_EQ(guid->ToString(), kText);
}

TEST(GuidTest, FromStringAcceptsLowerCase) {
  EXPECT_EQ(Guid::FromString("c496578a-0dfe-4b8f-870a-745238c6aeae"),
            Guid::FromString(kText));
}

TEST(GuidTest, FromStringRejectsOtherForms) {
  EXPECT_FALSE(Guid::FromString("").has_value());
  EXPECT_FALSE(
      Guid::FromString("{C496578A-0DFE-4B8F-870A-745238C6AEAE}").has_value());
  EXPECT_FALSE(
      Guid::FromString("C496578A_0DFE-4B8F-870A-745238C6AEAE").has_value());
  EXPECT_FALSE(
      Guid::FromString("G496578A-0DFE-4B8F-870A-745238C6AEAE").has_value());
}

// Part 6 §5.2.2.6: Data1..Data3 little-endian, then Data4 as written.
TEST(GuidTest, BytesAreInOpcUaBinaryOrder) {
  const Guid guid = *Guid::FromString(kText);
  const std::array<std::uint8_t, 16> expected = {
      0x8A, 0x57, 0x96, 0xC4, 0xFE, 0x0D, 0x8F, 0x4B,
      0x87, 0x0A, 0x74, 0x52, 0x38, 0xC6, 0xAE, 0xAE};
  EXPECT_EQ(guid.ToBytes(), expected);
}

TEST(GuidTest, BytesRoundTrip) {
  const Guid guid = *Guid::FromString(kText);
  const std::array<std::uint8_t, 16> bytes = guid.ToBytes();
  EXPECT_EQ(Guid::FromBytes(std::string_view{
                reinterpret_cast<const char*>(bytes.data()), bytes.size()}),
            guid);
}

TEST(GuidTest, FromBytesRejectsWrongLength) {
  EXPECT_FALSE(Guid::FromBytes(std::string(15, '\0')).has_value());
  EXPECT_FALSE(Guid::FromBytes(std::string(17, '\0')).has_value());
}

}  // namespace
}  // namespace scada
