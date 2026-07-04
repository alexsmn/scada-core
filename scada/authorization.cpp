#include "scada/authorization.h"

#include <algorithm>

namespace scada {

namespace {

constexpr std::uint32_t Bit(Privilege privilege) {
  return std::uint32_t{1} << static_cast<int>(privilege);
}

constexpr bool HasPrivilege(std::uint32_t access_rights, Privilege privilege) {
  return (access_rights & Bit(privilege)) != 0;
}

// Read-only baseline shared by Anonymous and Observer.
constexpr Permission kReadOnly =
    Permission::kBrowse | Permission::kRead | Permission::kReceiveEvents;

// Everything a full administrator (Configure) may do.
constexpr Permission kAdmin =
    kReadOnly | Permission::kReadHistory | Permission::kReadRolePermissions |
    Permission::kWrite | Permission::kWriteAttribute | Permission::kCall |
    Permission::kAddNode | Permission::kDeleteNode | Permission::kAddReference |
    Permission::kRemoveReference | Permission::kInsertHistory |
    Permission::kModifyHistory | Permission::kDeleteHistory |
    Permission::kWriteHistorizing | Permission::kWriteRolePermissions;

}  // namespace

std::vector<WellKnownRole> RolesForUser(std::uint32_t access_rights,
                                        bool is_anonymous) {
  if (is_anonymous) {
    return {WellKnownRole::kAnonymous};
  }

  std::vector<WellKnownRole> roles{WellKnownRole::kAuthenticatedUser,
                                   WellKnownRole::kObserver};
  if (HasPrivilege(access_rights, Privilege::Control)) {
    roles.push_back(WellKnownRole::kOperator);
  }
  if (HasPrivilege(access_rights, Privilege::Configure)) {
    roles.push_back(WellKnownRole::kEngineer);
    roles.push_back(WellKnownRole::kSupervisor);
    roles.push_back(WellKnownRole::kConfigureAdmin);
    roles.push_back(WellKnownRole::kSecurityAdmin);
  }
  return roles;
}

Permission DefaultPermissionsForRole(WellKnownRole role) {
  switch (role) {
    case WellKnownRole::kAnonymous:
      return kReadOnly;
    case WellKnownRole::kAuthenticatedUser:
    case WellKnownRole::kObserver:
      return kReadOnly | Permission::kReadHistory |
             Permission::kReadRolePermissions;
    case WellKnownRole::kOperator:
      return kReadOnly | Permission::kReadHistory |
             Permission::kReadRolePermissions | Permission::kWrite |
             Permission::kCall;
    case WellKnownRole::kEngineer:
    case WellKnownRole::kSupervisor:
    case WellKnownRole::kConfigureAdmin:
    case WellKnownRole::kSecurityAdmin:
      return kAdmin;
  }
  return Permission::kNone;
}

Permission PermissionsForUser(std::uint32_t access_rights, bool is_anonymous) {
  Permission permissions = Permission::kNone;
  for (const WellKnownRole role : RolesForUser(access_rights, is_anonymous)) {
    permissions |= DefaultPermissionsForRole(role);
  }
  return permissions;
}

bool IsPermitted(std::uint32_t access_rights,
                 bool is_anonymous,
                 Permission required) {
  return Contains(PermissionsForUser(access_rights, is_anonymous), required);
}

std::uint8_t UserAccessLevel(std::uint8_t access_level,
                             std::uint32_t access_rights,
                             bool is_anonymous) {
  const Permission permissions =
      PermissionsForUser(access_rights, is_anonymous);

  std::uint8_t user = access_level;
  const auto clear_unless = [&](std::uint8_t bit, Permission needed) {
    if (!Contains(permissions, needed)) {
      user &= static_cast<std::uint8_t>(~bit);
    }
  };

  clear_unless(access_level::kCurrentRead, Permission::kRead);
  clear_unless(access_level::kCurrentWrite, Permission::kWrite);
  clear_unless(access_level::kStatusWrite, Permission::kWrite);
  clear_unless(access_level::kTimestampWrite, Permission::kWrite);
  clear_unless(access_level::kHistoryRead, Permission::kReadHistory);

  // HistoryWrite covers inserting, replacing/updating, and removing history, so
  // it is retained when the caller holds any of those permissions.
  const Permission history_write = Permission::kInsertHistory |
                                   Permission::kModifyHistory |
                                   Permission::kDeleteHistory;
  if ((permissions & history_write) == Permission::kNone) {
    user &= static_cast<std::uint8_t>(~access_level::kHistoryWrite);
  }

  return user;
}

NodeId WellKnownRoleId(WellKnownRole role) {
  // Namespace-0 numeric ids of the standard Role objects (OPC UA Part 3 §4.9).
  constexpr NamespaceIndex kNs0 = 0;
  switch (role) {
    case WellKnownRole::kAnonymous:
      return NodeId{15644, kNs0};
    case WellKnownRole::kAuthenticatedUser:
      return NodeId{15656, kNs0};
    case WellKnownRole::kObserver:
      return NodeId{15668, kNs0};
    case WellKnownRole::kOperator:
      return NodeId{15680, kNs0};
    case WellKnownRole::kEngineer:
      return NodeId{15716, kNs0};
    case WellKnownRole::kSupervisor:
      return NodeId{15728, kNs0};
    case WellKnownRole::kConfigureAdmin:
      return NodeId{15704, kNs0};
    case WellKnownRole::kSecurityAdmin:
      return NodeId{15692, kNs0};
  }
  return NodeId{};
}

namespace {

std::vector<RolePermissionType> RolePermissionsFor(
    const std::vector<WellKnownRole>& roles) {
  std::vector<RolePermissionType> result;
  result.reserve(roles.size());
  for (const WellKnownRole role : roles) {
    result.push_back({.role_id = WellKnownRoleId(role),
                      .permissions = DefaultPermissionsForRole(role)});
  }
  return result;
}

}  // namespace

std::vector<RolePermissionType> DefaultRolePermissions() {
  static constexpr WellKnownRole kAllRoles[] = {
      WellKnownRole::kAnonymous,      WellKnownRole::kAuthenticatedUser,
      WellKnownRole::kObserver,       WellKnownRole::kOperator,
      WellKnownRole::kEngineer,       WellKnownRole::kSupervisor,
      WellKnownRole::kConfigureAdmin, WellKnownRole::kSecurityAdmin};
  return RolePermissionsFor({std::begin(kAllRoles), std::end(kAllRoles)});
}

std::vector<RolePermissionType> UserRolePermissions(std::uint32_t access_rights,
                                                    bool is_anonymous) {
  return RolePermissionsFor(RolesForUser(access_rights, is_anonymous));
}

std::vector<RolePermissionType> UserRolePermissionsFrom(
    std::span<const RolePermissionType> role_permissions,
    std::uint32_t access_rights,
    bool is_anonymous) {
  std::vector<NodeId> caller_role_ids;
  for (const WellKnownRole role : RolesForUser(access_rights, is_anonymous)) {
    caller_role_ids.push_back(WellKnownRoleId(role));
  }

  std::vector<RolePermissionType> result;
  for (const RolePermissionType& entry : role_permissions) {
    if (std::find(caller_role_ids.begin(), caller_role_ids.end(),
                  entry.role_id) != caller_role_ids.end()) {
      result.push_back(entry);
    }
  }
  return result;
}

Permission PermissionsForUserFrom(
    std::span<const RolePermissionType> role_permissions,
    std::uint32_t access_rights,
    bool is_anonymous) {
  Permission permissions = Permission::kNone;
  for (const RolePermissionType& entry :
       UserRolePermissionsFrom(role_permissions, access_rights, is_anonymous)) {
    permissions |= entry.permissions;
  }
  return permissions;
}

}  // namespace scada
