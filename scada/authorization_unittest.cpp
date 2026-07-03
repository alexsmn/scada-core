#include "scada/authorization.h"

#include <algorithm>

#include <gtest/gtest.h>

namespace scada {

namespace {

constexpr std::uint32_t kConfigure =
    std::uint32_t{1} << static_cast<int>(Privilege::Configure);
constexpr std::uint32_t kControl =
    std::uint32_t{1} << static_cast<int>(Privilege::Control);
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
  EXPECT_TRUE(IsPermitted(kConfigure, false, Permission::kWriteRolePermissions));
}

TEST(AuthorizationTest, RootHasAllPermissions) {
  EXPECT_TRUE(IsPermitted(kRootRights, false, Permission::kBrowse));
  EXPECT_TRUE(IsPermitted(kRootRights, false, Permission::kWrite));
  EXPECT_TRUE(IsPermitted(kRootRights, false, Permission::kCall));
  EXPECT_TRUE(IsPermitted(kRootRights, false, Permission::kAddNode));
  EXPECT_TRUE(IsPermitted(kRootRights, false, Permission::kDeleteHistory));
  EXPECT_TRUE(IsPermitted(kRootRights, false, Permission::kWriteRolePermissions));
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
