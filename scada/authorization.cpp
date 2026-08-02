#include "scada/authorization.h"

#include <algorithm>

namespace scada {

namespace {

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
  if (HasAccessRight(access_rights, AccessRight::kControl)) {
    roles.push_back(WellKnownRole::kOperator);
  }
  if (HasAccessRight(access_rights, AccessRight::kConfigure)) {
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
  // Namespace-0 numeric ids of the standard WellKnownRole_* objects, verified
  // against the official 1.05 NodeIds.csv
  // (https://files.opcfoundation.org/schemas/UA/1.05/NodeIds.csv): Anonymous
  // 15644, AuthenticatedUser 15656, Observer 15668, Operator 15680, Engineer
  // 16036, Supervisor 15692, SecurityAdmin 15704, ConfigureAdmin 15716.
  // OPC UA Part 3 §4.9 well-known roles,
  // https://reference.opcfoundation.org/Core/Part3/v105/docs/4.9
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
      return NodeId{16036, kNs0};
    case WellKnownRole::kSupervisor:
      return NodeId{15692, kNs0};
    case WellKnownRole::kConfigureAdmin:
      return NodeId{15716, kNs0};
    case WellKnownRole::kSecurityAdmin:
      return NodeId{15704, kNs0};
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

Permission PermissionsForRoles(
    std::span<const RolePermissionType> role_permissions,
    std::span<const NodeId> caller_role_ids) {
  // Effective permission = OR over the entries of the roles the caller holds
  // (OPC UA Part 3 §4.9,
  // https://reference.opcfoundation.org/Core/Part3/v105/docs/4.9).
  Permission permissions = Permission::kNone;
  for (const RolePermissionType& entry : role_permissions) {
    if (std::find(caller_role_ids.begin(), caller_role_ids.end(),
                  entry.role_id) != caller_role_ids.end()) {
      permissions |= entry.permissions;
    }
  }
  return permissions;
}

PasswordPolicyViolation CheckPasswordPolicy(std::string_view password,
                                            PasswordOptions options,
                                            double min_length,
                                            double max_length) {
  // Length first: a Range whose bound is non-positive means "unconstrained",
  // which is how a server that publishes no minimum or no maximum shows up
  // (OPC UA Part 18 §5.2.2 PasswordLength).
  const double length = static_cast<double>(password.size());
  if (min_length > 0 && length < min_length) {
    return PasswordPolicyViolation::kTooShort;
  }
  if (max_length > 0 && length > max_length) {
    return PasswordPolicyViolation::kTooLong;
  }

  // The kRequires* bits are defined over ASCII (Part 18 §5.2.2), so classify
  // by byte rather than by locale: a UTF-8 continuation byte is not an upper
  // case letter in any locale the policy means to describe, and running this
  // through <cctype> with a non-"C" locale would make the same password pass
  // on one host and fail on another.
  bool has_upper = false;
  bool has_lower = false;
  bool has_digit = false;
  bool has_special = false;
  for (const char c : password) {
    if (c >= 'A' && c <= 'Z') {
      has_upper = true;
    } else if (c >= 'a' && c <= 'z') {
      has_lower = true;
    } else if (c >= '0' && c <= '9') {
      has_digit = true;
    } else {
      has_special = true;
    }
  }

  if (HasPasswordOption(options, PasswordOptions::kRequiresUpperCaseCharacters) &&
      !has_upper) {
    return PasswordPolicyViolation::kMissingUpperCase;
  }
  if (HasPasswordOption(options, PasswordOptions::kRequiresLowerCaseCharacters) &&
      !has_lower) {
    return PasswordPolicyViolation::kMissingLowerCase;
  }
  if (HasPasswordOption(options, PasswordOptions::kRequiresDigitCharacters) &&
      !has_digit) {
    return PasswordPolicyViolation::kMissingDigit;
  }
  if (HasPasswordOption(options, PasswordOptions::kRequiresSpecialCharacters) &&
      !has_special) {
    return PasswordPolicyViolation::kMissingSpecial;
  }
  return PasswordPolicyViolation::kNone;
}

bool IsWellKnownRoleId(const NodeId& role_id) {
  for (const WellKnownRole role :
       {WellKnownRole::kAnonymous, WellKnownRole::kAuthenticatedUser,
        WellKnownRole::kObserver, WellKnownRole::kOperator,
        WellKnownRole::kEngineer, WellKnownRole::kSupervisor,
        WellKnownRole::kConfigureAdmin, WellKnownRole::kSecurityAdmin}) {
    if (WellKnownRoleId(role) == role_id) {
      return true;
    }
  }
  return false;
}

std::uint32_t AccessRightsForRoles(std::span<const NodeId> role_ids) {
  std::uint32_t access_rights = 0;
  for (const NodeId& role_id : role_ids) {
    if (role_id == WellKnownRoleId(WellKnownRole::kOperator)) {
      access_rights |= AccessRightBit(AccessRight::kControl);
    } else if (role_id == WellKnownRoleId(WellKnownRole::kEngineer) ||
               role_id == WellKnownRoleId(WellKnownRole::kSupervisor) ||
               role_id == WellKnownRoleId(WellKnownRole::kConfigureAdmin) ||
               role_id == WellKnownRoleId(WellKnownRole::kSecurityAdmin)) {
      access_rights |= AccessRightBit(AccessRight::kConfigure);
    }
  }
  return access_rights;
}

std::vector<NodeId> CallerRoleIds(std::uint32_t access_rights,
                                  bool is_anonymous,
                                  std::span<const NodeId> granted_role_ids) {
  std::vector<NodeId> role_ids;
  for (const WellKnownRole role : RolesForUser(access_rights, is_anonymous)) {
    role_ids.push_back(WellKnownRoleId(role));
  }
  role_ids.insert(role_ids.end(), granted_role_ids.begin(),
                  granted_role_ids.end());
  return role_ids;
}

}  // namespace scada
