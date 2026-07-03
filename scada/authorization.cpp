#include "scada/authorization.h"

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

}  // namespace scada
