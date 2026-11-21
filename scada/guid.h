#pragma once

#include "scada/string.h"

#include <array>
#include <compare>
#include <cstdint>
#include <optional>
#include <ostream>
#include <string_view>

namespace scada {

// A 16-byte globally unique identifier, as the OPC UA Guid built-in type. The
// four fields mirror the structure the spec defines, which is also the order
// the OPC UA Binary encoding writes (Data1/Data2/Data3 little-endian, then
// Data4 verbatim), so no byte-order convention is left to callers. It exists
// so that a Guid NodeId from a third-party OPC UA server survives the bridge
// (backlog 826). OPC UA Part 3 §8.16 Guid,
// https://reference.opcfoundation.org/Core/Part3/v105/docs/8.16
struct Guid {
  std::uint32_t data1 = 0;
  std::uint16_t data2 = 0;
  std::uint16_t data3 = 0;
  std::array<std::uint8_t, 8> data4 = {};

  // Whether every field is zero.
  constexpr bool is_null() const noexcept { return *this == Guid{}; }

  constexpr bool operator==(const Guid& other) const noexcept = default;
  constexpr std::strong_ordering operator<=>(const Guid& other) const noexcept =
      default;

  // The canonical 36-character text form, upper case and hyphen separated:
  // "C496578A-0DFE-4B8F-870A-745238C6AEAE". OPC UA Part 6 §5.1.3 Guid,
  // https://reference.opcfoundation.org/Core/Part6/v105/docs/5.1.3
  String ToString() const;

  // Parses the canonical text form. Accepts either case; returns nullopt for
  // anything else, including the brace-wrapped Microsoft form.
  static std::optional<Guid> FromString(std::string_view text);

  // The 16 bytes in OPC UA Binary order: Data1, Data2 and Data3 little-endian,
  // then Data4. OPC UA Part 6 §5.2.2.6 Guid,
  // https://reference.opcfoundation.org/Core/Part6/v105/docs/5.2.2.6
  std::array<std::uint8_t, 16> ToBytes() const noexcept;

  // The inverse of ToBytes(). Returns nullopt unless `bytes` is exactly 16
  // bytes long.
  static std::optional<Guid> FromBytes(std::string_view bytes) noexcept;
};

inline std::ostream& operator<<(std::ostream& stream, const Guid& guid) {
  return stream << '"' << guid.ToString() << '"';
}

}  // namespace scada
