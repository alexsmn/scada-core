#pragma once

#include <cstdint>

namespace scada {

// The coarse per-account access-rights bits, stored as the
// `UserType.AccessRights` property and carried on a session as
// `ServiceContext::user_rights`. They are the *account* model only: a caller's
// enforceable rights are OPC UA permissions (`scada::Permission`, Part 3
// §8.55), derived from these bits through the well-known role map in
// `scada/authorization.h` — Control grants the Operator role, Configure the
// Engineer / Supervisor / ConfigureAdmin / SecurityAdmin roles.
enum class AccessRight { kConfigure = 0, kControl = 1 };

// The bitmask bit of a single access right.
constexpr std::uint32_t AccessRightBit(AccessRight right) {
  return std::uint32_t{1} << static_cast<int>(right);
}

// True when the `access_rights` bitmask grants `right`.
constexpr bool HasAccessRight(std::uint32_t access_rights, AccessRight right) {
  return (access_rights & AccessRightBit(right)) != 0;
}

}  // namespace scada
