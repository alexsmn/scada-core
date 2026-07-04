#pragma once

#include "scada/node_id.h"
#include "scada/privileges.h"

#include <cstdint>
#include <vector>

namespace scada {

// OPC UA well-known roles (OPC UA Part 3 §4.8 / Part 18). The server derives a
// caller's roles from its coarse access-rights bitmask (scada::Privilege), so
// the standard role/permission surface can be exposed without a per-user role
// store. https://reference.opcfoundation.org/Core/Part3/v105/docs/4.8
enum class WellKnownRole {
  kAnonymous,
  kAuthenticatedUser,
  kObserver,
  kOperator,
  kEngineer,
  kSupervisor,
  kConfigureAdmin,
  kSecurityAdmin,
};

// PermissionType bits (OPC UA Part 3 §8.55). The numeric values match the OPC UA
// PermissionType bit mask so they can be surfaced directly on the wire.
// https://reference.opcfoundation.org/Core/Part3/v105/docs/8.55
enum class Permission : std::uint32_t {
  kNone = 0,
  kBrowse = 1u << 0,
  kReadRolePermissions = 1u << 1,
  kWriteAttribute = 1u << 2,
  kWriteRolePermissions = 1u << 3,
  kWriteHistorizing = 1u << 4,
  kRead = 1u << 5,
  kWrite = 1u << 6,
  kReadHistory = 1u << 7,
  kInsertHistory = 1u << 8,
  kModifyHistory = 1u << 9,
  kDeleteHistory = 1u << 10,
  kReceiveEvents = 1u << 11,
  kCall = 1u << 12,
  kAddReference = 1u << 13,
  kRemoveReference = 1u << 14,
  kDeleteNode = 1u << 15,
  kAddNode = 1u << 16,
};

// OPC UA AccessLevelType bit mask (OPC UA Part 3 §8.57). The numeric values
// match the wire encoding, so an AccessLevel / UserAccessLevel byte can be
// composed from these and returned directly.
// https://reference.opcfoundation.org/Core/Part3/v105/docs/8.57
namespace access_level {
inline constexpr std::uint8_t kNone = 0x00;
inline constexpr std::uint8_t kCurrentRead = 0x01;
inline constexpr std::uint8_t kCurrentWrite = 0x02;
inline constexpr std::uint8_t kHistoryRead = 0x04;
inline constexpr std::uint8_t kHistoryWrite = 0x08;
inline constexpr std::uint8_t kSemanticChange = 0x10;
inline constexpr std::uint8_t kStatusWrite = 0x20;
inline constexpr std::uint8_t kTimestampWrite = 0x40;
}  // namespace access_level

constexpr Permission operator|(Permission a, Permission b) {
  return static_cast<Permission>(static_cast<std::uint32_t>(a) |
                                 static_cast<std::uint32_t>(b));
}

constexpr Permission operator&(Permission a, Permission b) {
  return static_cast<Permission>(static_cast<std::uint32_t>(a) &
                                 static_cast<std::uint32_t>(b));
}

constexpr Permission& operator|=(Permission& a, Permission b) {
  return a = a | b;
}

// True if every bit in `required` is set in `granted`.
constexpr bool Contains(Permission granted, Permission required) {
  return (granted & required) == required;
}

// Derives the well-known roles a caller holds from its access-rights bitmask
// (bits from scada::Privilege) and whether the session is anonymous. An
// anonymous session is the Anonymous role only; any authenticated caller is at
// least AuthenticatedUser + Observer, gaining Operator with the Control
// privilege and the admin/engineering roles with the Configure privilege.
std::vector<WellKnownRole> RolesForUser(std::uint32_t access_rights,
                                        bool is_anonymous);

// The default permissions granted to a single well-known role (the
// DefaultRolePermissions the server publishes per namespace, Part 3 §5.2.9).
Permission DefaultPermissionsForRole(WellKnownRole role);

// The union of default permissions across all roles a caller holds — its
// effective permission set before any per-node RolePermissions override.
Permission PermissionsForUser(std::uint32_t access_rights, bool is_anonymous);

// True if the caller's effective permissions include `required`.
bool IsPermitted(std::uint32_t access_rights,
                 bool is_anonymous,
                 Permission required);

// Narrows a node's `access_level` (AccessLevelType bits) to the caller's
// UserAccessLevel: each access bit is kept only when the caller holds the
// corresponding permission (OPC UA Part 3 §5.6.2 UserAccessLevel). CurrentRead
// requires kRead; CurrentWrite / StatusWrite / TimestampWrite require kWrite;
// HistoryRead requires kReadHistory; HistoryWrite requires any history-modifying
// permission (Insert/Modify/Delete). SemanticChange is informational and left as
// declared. https://reference.opcfoundation.org/Core/Part3/v105/docs/5.6.2
std::uint8_t UserAccessLevel(std::uint8_t access_level,
                             std::uint32_t access_rights,
                             bool is_anonymous);

// One (role, permissions) entry of a node's RolePermissions /
// UserRolePermissions attribute (OPC UA Part 3 §8.56 RolePermissionType).
// https://reference.opcfoundation.org/Core/Part3/v105/docs/8.56
struct RolePermissionType {
  NodeId role_id;
  Permission permissions = Permission::kNone;

  friend bool operator==(const RolePermissionType&,
                         const RolePermissionType&) = default;
};

// The namespace-0 well-known Role NodeId for `role` (OPC UA Part 3 §4.9;
// standard NodeIds, e.g. WellKnownRole_Operator = i=15680).
// https://reference.opcfoundation.org/Core/Part3/v105/docs/4.9
NodeId WellKnownRoleId(WellKnownRole role);

// The default RolePermissions a node publishes: every well-known role paired
// with its default permission set. This is the admin-visible RolePermissions
// attribute (OPC UA Part 3 §5.2.9), independent of the caller.
std::vector<RolePermissionType> DefaultRolePermissions();

// The caller's UserRolePermissions: the roles the caller currently holds, each
// paired with its effective permissions (OPC UA Part 3 §5.2.10). This is the
// per-session narrowing of RolePermissions.
std::vector<RolePermissionType> UserRolePermissions(std::uint32_t access_rights,
                                                    bool is_anonymous);

}  // namespace scada
