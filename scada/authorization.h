#pragma once

#include "scada/access_rights.h"
#include "scada/node_id.h"

#include <cstdint>
#include <span>
#include <string>
#include <vector>

namespace scada {

// OPC UA well-known roles (OPC UA Part 3 §4.8 / Part 18). The server derives a
// caller's roles from its coarse access-rights bitmask (scada::AccessRight), so
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

// PermissionType bits (OPC UA Part 3 §8.55). The numeric values match the OPC
// UA PermissionType bit mask so they can be surfaced directly on the wire.
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
// (bits from scada::AccessRight) and whether the session is anonymous. An
// anonymous session is the Anonymous role only; any authenticated caller is at
// least AuthenticatedUser + Observer, gaining Operator with the Control
// access right and the admin/engineering roles with the Configure access right.
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
// HistoryRead requires kReadHistory; HistoryWrite requires any
// history-modifying permission (Insert/Modify/Delete). SemanticChange is
// informational and left as declared.
// https://reference.opcfoundation.org/Core/Part3/v105/docs/5.6.2
std::uint8_t UserAccessLevel(std::uint8_t access_level,
                             std::uint32_t access_rights,
                             bool is_anonymous);

// IdentityCriteriaType: how an identity mapping rule matches a session's
// identity token (OPC UA Part 18 §4.4.4). The numeric values match the OPC UA
// enumeration so they can be stored and surfaced on the wire directly. Only
// kUserName is evaluated for server-managed username/password accounts;
// kGroupId/kRole refer to claims in an external Authorization Service's
// Access Token and are not applicable to UserNameIdentityTokens.
// https://reference.opcfoundation.org/Core/Part18/v105/docs/4.4.4
enum class IdentityCriteriaType : std::int32_t {
  kUserName = 1,
  kThumbprint = 2,
  kRole = 3,
  kGroupId = 4,
  kAnonymous = 5,
  kAuthenticatedUser = 6,
  kApplication = 7,
  kX509Subject = 8,
  kTrustedApplication = 9,
};

// One identity mapping rule of a Role's Identities property (OPC UA Part 18
// §4.4.3 IdentityMappingRuleType): a criteria type plus the criteria string
// (for kUserName, the user's name; empty for the token-class criteria).
// https://reference.opcfoundation.org/Core/Part18/v105/docs/4.4.3
struct IdentityMappingRule {
  IdentityCriteriaType criteria_type = IdentityCriteriaType::kUserName;
  std::string criteria;

  friend bool operator==(const IdentityMappingRule&,
                         const IdentityMappingRule&) = default;
};

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

// The caller's UserRolePermissions narrowed from a specific node's
// `role_permissions` override (Part 3 §5.2.10): the entries whose role the
// caller currently holds. Use this when a node carries its own RolePermissions
// instead of the server default.
std::vector<RolePermissionType> UserRolePermissionsFrom(
    std::span<const RolePermissionType> role_permissions,
    std::uint32_t access_rights,
    bool is_anonymous);

// The caller's effective permissions derived from a specific node's
// `role_permissions` override: the union of permissions across the entries
// whose role the caller holds. This is the per-node analogue of
// `PermissionsForUser`.
Permission PermissionsForUserFrom(
    std::span<const RolePermissionType> role_permissions,
    std::uint32_t access_rights,
    bool is_anonymous);

// The union of permissions granted by `role_permissions` to a caller holding
// exactly `caller_role_ids` — the OR-across-granted-roles evaluation of OPC UA
// Part 3 §4.9 for an explicit role-id set (well-known role ids and custom
// group Role NodeIds alike). Used with a namespace's DefaultRolePermissions
// (Part 3 §5.2.9, Part 5 §6.3.13) to compute a caller's access to a
// namespace. https://reference.opcfoundation.org/Core/Part3/v105/docs/4.9
Permission PermissionsForRoles(
    std::span<const RolePermissionType> role_permissions,
    std::span<const NodeId> caller_role_ids);

// The caller's full role-id set: the well-known roles derived from
// `access_rights` (RolesForUser) plus the custom (group) Role NodeIds granted
// at session activation (OPC UA Part 18 §4.4.1).
std::vector<NodeId> CallerRoleIds(std::uint32_t access_rights,
                                  bool is_anonymous,
                                  std::span<const NodeId> granted_role_ids);

}  // namespace scada
