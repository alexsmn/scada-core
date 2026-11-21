#include "scada/guid.h"

#include <cstddef>
#include <format>

namespace scada {
namespace {

// Length of the canonical 8-4-4-4-12 text form.
constexpr std::size_t kTextLength = 36;

// Length of the binary form.
constexpr std::size_t kByteLength = 16;

std::optional<unsigned> ParseHexDigit(char c) {
  if (c >= '0' && c <= '9')
    return static_cast<unsigned>(c - '0');
  if (c >= 'a' && c <= 'f')
    return static_cast<unsigned>(c - 'a') + 10;
  if (c >= 'A' && c <= 'F')
    return static_cast<unsigned>(c - 'A') + 10;
  return std::nullopt;
}

// Reads `digits` hex characters starting at `offset` into an integer value.
std::optional<std::uint64_t> ParseHex(std::string_view text,
                                      std::size_t offset,
                                      std::size_t digits) {
  std::uint64_t value = 0;
  for (std::size_t i = 0; i < digits; ++i) {
    const std::optional<unsigned> digit = ParseHexDigit(text[offset + i]);
    if (!digit.has_value())
      return std::nullopt;
    value = (value << 4) | *digit;
  }
  return value;
}

}  // namespace

String Guid::ToString() const {
  return std::format(
      "{:08X}-{:04X}-{:04X}-{:02X}{:02X}-{:02X}{:02X}{:02X}{:02X}{:02X}{:02X}",
      data1, data2, data3, data4[0], data4[1], data4[2], data4[3], data4[4],
      data4[5], data4[6], data4[7]);
}

// static
std::optional<Guid> Guid::FromString(std::string_view text) {
  if (text.size() != kTextLength)
    return std::nullopt;
  if (text[8] != '-' || text[13] != '-' || text[18] != '-' || text[23] != '-')
    return std::nullopt;

  const std::optional<std::uint64_t> d1 = ParseHex(text, 0, 8);
  const std::optional<std::uint64_t> d2 = ParseHex(text, 9, 4);
  const std::optional<std::uint64_t> d3 = ParseHex(text, 14, 4);
  if (!d1.has_value() || !d2.has_value() || !d3.has_value())
    return std::nullopt;

  Guid guid;
  guid.data1 = static_cast<std::uint32_t>(*d1);
  guid.data2 = static_cast<std::uint16_t>(*d2);
  guid.data3 = static_cast<std::uint16_t>(*d3);

  // The last two groups hold Data4 as eight bytes: two in the 4-digit group at
  // offset 19, six in the 12-digit group at offset 24.
  for (std::size_t i = 0; i < guid.data4.size(); ++i) {
    const std::size_t offset = i < 2 ? 19 + i * 2 : 24 + (i - 2) * 2;
    const std::optional<std::uint64_t> byte = ParseHex(text, offset, 2);
    if (!byte.has_value())
      return std::nullopt;
    guid.data4[i] = static_cast<std::uint8_t>(*byte);
  }
  return guid;
}

std::array<std::uint8_t, 16> Guid::ToBytes() const noexcept {
  std::array<std::uint8_t, kByteLength> bytes = {};
  for (std::size_t i = 0; i < 4; ++i)
    bytes[i] = static_cast<std::uint8_t>(data1 >> (8 * i));
  for (std::size_t i = 0; i < 2; ++i) {
    bytes[4 + i] = static_cast<std::uint8_t>(data2 >> (8 * i));
    bytes[6 + i] = static_cast<std::uint8_t>(data3 >> (8 * i));
  }
  for (std::size_t i = 0; i < data4.size(); ++i)
    bytes[8 + i] = data4[i];
  return bytes;
}

// static
std::optional<Guid> Guid::FromBytes(std::string_view bytes) noexcept {
  if (bytes.size() != kByteLength)
    return std::nullopt;
  const auto byte = [&](std::size_t i) -> std::uint32_t {
    return static_cast<std::uint8_t>(bytes[i]);
  };
  Guid guid;
  guid.data1 = byte(0) | (byte(1) << 8) | (byte(2) << 16) | (byte(3) << 24);
  guid.data2 = static_cast<std::uint16_t>(byte(4) | (byte(5) << 8));
  guid.data3 = static_cast<std::uint16_t>(byte(6) | (byte(7) << 8));
  for (std::size_t i = 0; i < guid.data4.size(); ++i)
    guid.data4[i] = static_cast<std::uint8_t>(byte(8 + i));
  return guid;
}

}  // namespace scada
