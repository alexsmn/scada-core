#include "scada/authorization.h"

#include <algorithm>

#include <gtest/gtest.h>

namespace scada {

namespace {

constexpr std::uint32_t kConfigure = std::uint32_t{1}
                                     << static_cast<int>(Privilege::Configure);
constexpr std::uint32_t kControl = std::uint32_t{1}
                                   << static_cast<int>(Privilege::Control);
constexpr std::uint32_t kRootRights = kConfigure | kControl;

bool HasRole(std::uint32_t access_rights, bool anonymous, WellKnownRole role) {
  const auto roles = RolesForUser(access_rights, anonymous);
  return std::find(roles.begin(), roles.end(), role) != roles.end();
}

TEST(AuthorizationTest, AnonymousIsReadOnly) {
  EXPECT_TRUE(HasRole(0, /*anonymous=*/true, WellKnownRole::kAnonymous));
  EXPECT_FALSE(HasRole(0, /*anonymous=*/true, WellKnownRole::kObserver));

  EXPECT_TRUE(IsPermitted(0, true, Permission::kBrowse));
  EXPECT_TRUE(IsPermitted(0, true, Permission::kRead));
  EXPECT_TRUE(IsPermitted(0, true, Permission::kReceiveEvents));
  EXPECT_FALSE(IsPermitted(0, true, Permission::kWrite));
  EXPECT_FALSE(IsPermitted(0, true, Permission::kCall));
  EXPECT_FALSE(IsPermitted(0, true, Permission::kAddNode));
}

TEST(AuthorizationTest, AuthenticatedUserWithNoBitsIsObserver) {
  EXPECT_TRUE(HasRole(0, false, WellKnownRole::kAuthenticatedUser));
  EXPECT_TRUE(HasRole(0, false, WellKnownRole::kObserver));
  EXPECT_FALSE(HasRole(0, false, WellKnownRole::kOperator));

  EXPECT_TRUE(IsPermitted(0, false, Permission::kRead));
  EXPECT_TRUE(IsPermitted(0, false, Permission::kReadHistory));
  EXPECT_FALSE(IsPermitted(0, false, Permission::kWrite));
}

TEST(AuthorizationTest, ControlPrivilegeGrantsOperator) {
  EXPECT_TRUE(HasRole(kControl, false, WellKnownRole::kOperator));
  EXPECT_FALSE(HasRole(kControl, false, WellKnownRole::kConfigureAdmin));

  EXPECT_TRUE(IsPermitted(kControl, false, Permission::kWrite));
  EXPECT_TRUE(IsPermitted(kControl, false, Permission::kCall));
  // An operator cannot add or delete nodes (that is a configuration action).
  EXPECT_FALSE(IsPermitted(kControl, false, Permission::kAddNode));
  EXPECT_FALSE(IsPermitted(kControl, false, Permission::kWriteRolePermissions));
}

TEST(AuthorizationTest, ConfigurePrivilegeGrantsAdminRoles) {
  EXPECT_TRUE(HasRole(kConfigure, false, WellKnownRole::kConfigureAdmin));
  EXPECT_TRUE(HasRole(kConfigure, false, WellKnownRole::kSecurityAdmin));

  EXPECT_TRUE(IsPermitted(kConfigure, false, Permission::kWrite));
  EXPECT_TRUE(IsPermitted(kConfigure, false, Permission::kAddNode));
  EXPECT_TRUE(IsPermitted(kConfigure, false, Permission::kDeleteNode));
  EXPECT_TRUE(
      IsPermitted(kConfigure, false, Permission::kWriteRolePermissions));
}

TEST(AuthorizationTest, RootHasAllPermissions) {
  EXPECT_TRUE(IsPermitted(kRootRights, false, Permission::kBrowse));
  EXPECT_TRUE(IsPermitted(kRootRights, false, Permission::kWrite));
  EXPECT_TRUE(IsPermitted(kRootRights, false, Permission::kCall));
  EXPECT_TRUE(IsPermitted(kRootRights, false, Permission::kAddNode));
  EXPECT_TRUE(IsPermitted(kRootRights, false, Permission::kDeleteHistory));
  EXPECT_TRUE(
      IsPermitted(kRootRights, false, Permission::kWriteRolePermissions));
}

TEST(AuthorizationTest, UserAccessLevelNarrowsWriteToPermittedRoles) {
  constexpr std::uint8_t kReadWrite =
      access_level::kCurrentRead | access_level::kCurrentWrite;

  // An Operator (Control) keeps write; a plain authenticated Observer and an
  // anonymous caller lose it but keep read.
  EXPECT_EQ(UserAccessLevel(kReadWrite, kControl, false), kReadWrite);
  EXPECT_EQ(UserAccessLevel(kReadWrite, 0, false), access_level::kCurrentRead);
  EXPECT_EQ(UserAccessLevel(kReadWrite, 0, true), access_level::kCurrentRead);
}

TEST(AuthorizationTest, UserAccessLevelNarrowsHistoryAndWriteBits) {
  constexpr std::uint8_t kAll =
      access_level::kCurrentRead | access_level::kCurrentWrite |
      access_level::kHistoryRead | access_level::kHistoryWrite |
      access_level::kStatusWrite | access_level::kTimestampWrite;

  // Observer: current-read + history-read only (no write, no history write).
  EXPECT_EQ(UserAccessLevel(kAll, 0, false),
            access_level::kCurrentRead | access_level::kHistoryRead);
  // Anonymous has no history-read permission.
  EXPECT_EQ(UserAccessLevel(kAll, 0, true), access_level::kCurrentRead);
  // A configuration admin retains every bit (full write + history rights).
  EXPECT_EQ(UserAccessLevel(kAll, kConfigure, false), kAll);
}

TEST(AuthorizationTest, UserAccessLevelPreservesSemanticChange) {
  // SemanticChange is informational, not user-permission gated.
  EXPECT_EQ(UserAccessLevel(access_level::kSemanticChange, 0, /*anon=*/true),
            access_level::kSemanticChange);
}

TEST(AuthorizationTest, WellKnownRoleIdsAreNamespaceZero) {
  // Ids per the official 1.05 NodeIds.csv,
  // https://files.opcfoundation.org/schemas/UA/1.05/NodeIds.csv
  EXPECT_EQ(WellKnownRoleId(WellKnownRole::kOperator), (NodeId{15680, 0}));
  EXPECT_EQ(WellKnownRoleId(WellKnownRole::kSecurityAdmin), (NodeId{15704, 0}));
  EXPECT_EQ(WellKnownRoleId(WellKnownRole::kSupervisor), (NodeId{15692, 0}));
  EXPECT_EQ(WellKnownRoleId(WellKnownRole::kConfigureAdmin),
            (NodeId{15716, 0}));
  EXPECT_EQ(WellKnownRoleId(WellKnownRole::kEngineer), (NodeId{16036, 0}));
  EXPECT_EQ(WellKnownRoleId(WellKnownRole::kAnonymous), (NodeId{15644, 0}));
}

TEST(AuthorizationTest, DefaultRolePermissionsCoversAllRoles) {
  const auto entries = DefaultRolePermissions();
  ASSERT_EQ(entries.size(), 8u);

  // Anonymous is read-only; the security admin has write-role-permissions.
  const auto find = [&](WellKnownRole role) {
    const NodeId id = WellKnownRoleId(role);
    for (const auto& entry : entries) {
      if (entry.role_id == id)
        return entry.permissions;
    }
    return Permission::kNone;
  };
  EXPECT_TRUE(Contains(find(WellKnownRole::kAnonymous), Permission::kRead));
  EXPECT_FALSE(Contains(find(WellKnownRole::kAnonymous), Permission::kWrite));
  EXPECT_TRUE(Contains(find(WellKnownRole::kOperator), Permission::kWrite));
  EXPECT_TRUE(Contains(find(WellKnownRole::kSecurityAdmin),
                       Permission::kWriteRolePermissions));
}

TEST(AuthorizationTest, UserRolePermissionsReflectsCallerRoles) {
  // An anonymous caller holds only the Anonymous role.
  const auto anon = UserRolePermissions(0, /*is_anonymous=*/true);
  ASSERT_EQ(anon.size(), 1u);
  EXPECT_EQ(anon.front().role_id, WellKnownRoleId(WellKnownRole::kAnonymous));

  // An operator (Control) holds AuthenticatedUser, Observer and Operator, and
  // its Operator entry carries the write/call permissions.
  const auto op = UserRolePermissions(kControl, /*is_anonymous=*/false);
  EXPECT_EQ(op.size(), 3u);
  const auto has_operator_with_write =
      std::any_of(op.begin(), op.end(), [](const RolePermissionType& entry) {
        return entry.role_id == WellKnownRoleId(WellKnownRole::kOperator) &&
               Contains(entry.permissions, Permission::kWrite);
      });
  EXPECT_TRUE(has_operator_with_write);
}

TEST(AuthorizationTest, UserRolePermissionsFromNodeOverride) {
  // A node override that grants Operator full write but Observer only browse.
  const std::vector<RolePermissionType> override_permissions = {
      {.role_id = WellKnownRoleId(WellKnownRole::kObserver),
       .permissions = Permission::kBrowse},
      {.role_id = WellKnownRoleId(WellKnownRole::kOperator),
       .permissions =
           Permission::kBrowse | Permission::kRead | Permission::kWrite}};

  // An operator holds AuthenticatedUser + Observer + Operator; only the
  // Observer and Operator entries appear in the override, and its effective
  // permissions are the union (so it may write).
  const auto op = UserRolePermissionsFrom(override_permissions, kControl,
                                          /*is_anonymous=*/false);
  EXPECT_EQ(op.size(), 2u);
  EXPECT_TRUE(Contains(PermissionsForUserFrom(override_permissions, kControl,
                                              /*is_anonymous=*/false),
                       Permission::kWrite));

  // A plain authenticated Observer matches only the Observer entry and may not
  // write, even though the node grants Operator write.
  const auto observer = UserRolePermissionsFrom(override_permissions, 0,
                                                /*is_anonymous=*/false);
  ASSERT_EQ(observer.size(), 1u);
  EXPECT_EQ(observer.front().role_id,
            WellKnownRoleId(WellKnownRole::kObserver));
  EXPECT_FALSE(Contains(
      PermissionsForUserFrom(override_permissions, 0, /*is_anonymous=*/false),
      Permission::kWrite));

  // An anonymous caller matches no entry in this override.
  EXPECT_TRUE(
      UserRolePermissionsFrom(override_permissions, 0, /*is_anonymous=*/true)
          .empty());
}

TEST(AuthorizationTest, PermissionBitwiseHelpers) {
  const Permission combined = Permission::kRead | Permission::kWrite;
  EXPECT_TRUE(Contains(combined, Permission::kRead));
  EXPECT_TRUE(Contains(combined, Permission::kWrite));
  EXPECT_TRUE(Contains(combined, Permission::kRead | Permission::kWrite));
  EXPECT_FALSE(Contains(combined, Permission::kCall));
}

}  // namespace

}  // namespace scada
