#include "scada/authorization.h"

#include <algorithm>

#include <gtest/gtest.h>

namespace scada {

namespace {

constexpr std::uint32_t kConfigure = AccessRightBit(AccessRight::kConfigure);
constexpr std::uint32_t kControl = AccessRightBit(AccessRight::kControl);
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

TEST(AuthorizationTest, ControlAccessRightGrantsOperator) {
  EXPECT_TRUE(HasRole(kControl, false, WellKnownRole::kOperator));
  EXPECT_FALSE(HasRole(kControl, false, WellKnownRole::kConfigureAdmin));

  EXPECT_TRUE(IsPermitted(kControl, false, Permission::kWrite));
  EXPECT_TRUE(IsPermitted(kControl, false, Permission::kCall));
  // An operator cannot add or delete nodes (that is a configuration action).
  EXPECT_FALSE(IsPermitted(kControl, false, Permission::kAddNode));
  EXPECT_FALSE(IsPermitted(kControl, false, Permission::kWriteRolePermissions));
}

TEST(AuthorizationTest, ConfigureAccessRightGrantsAdminRoles) {
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

// The coarse capabilities both clients present are derived from the SAME map
// the server enforces with, by evaluating the published RolePermissions rather
// than by consulting a client-side table. This test is the contract that lets
// each client drop its own copy of the map: whatever
// `DefaultPermissionsForRole` says, evaluating the published entries has to
// agree with it.
TEST(AuthorizationTest, CapabilitiesFollowThePublishedRoleMap) {
  const std::vector<RolePermissionType> published = DefaultRolePermissions();

  const auto granted = [&](WellKnownRole role, Capability capability) {
    const NodeId role_ids[] = {WellKnownRoleId(role)};
    return Grants(PermissionsForRoles(published, role_ids), capability);
  };

  // Observer looks; Operator also acts; ConfigureAdmin also configures.
  EXPECT_TRUE(granted(WellKnownRole::kObserver, Capability::kView));
  EXPECT_FALSE(granted(WellKnownRole::kObserver, Capability::kControl));
  EXPECT_FALSE(granted(WellKnownRole::kObserver, Capability::kConfigure));

  EXPECT_TRUE(granted(WellKnownRole::kOperator, Capability::kControl));
  EXPECT_FALSE(granted(WellKnownRole::kOperator, Capability::kConfigure));

  EXPECT_TRUE(granted(WellKnownRole::kConfigureAdmin, Capability::kConfigure));

  // Every role's evaluated capabilities must match its enforced permissions --
  // the property that makes a client reading the map incapable of disagreeing
  // with the server.
  for (const RolePermissionType& entry : published) {
    for (const Capability capability :
         {Capability::kView, Capability::kControl, Capability::kConfigure}) {
      const NodeId role_ids[] = {entry.role_id};
      EXPECT_EQ(Grants(PermissionsForRoles(published, role_ids), capability),
                Grants(entry.permissions, capability))
          << "role " << entry.role_id.ToString();
    }
  }
}

// A Role the server published no entry for grants nothing. This is what keeps
// a custom (group) Role honest on a client: its grants are a per-namespace
// policy (Part 3 §5.2.9) that is not in this map, and inventing one would let
// the client overstate what an account may do.
TEST(AuthorizationTest, UnpublishedRoleGrantsNothing) {
  const std::vector<RolePermissionType> published = DefaultRolePermissions();
  const NodeId custom_role[] = {NodeId{4242, 7}};

  const Permission permissions = PermissionsForRoles(published, custom_role);

  EXPECT_EQ(permissions, Permission::kNone);
  EXPECT_FALSE(Grants(permissions, Capability::kView));
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

// AccessRightsForRoles is the inverse of RolesForUser, and roles are now the
// stored model, so a round trip through it must not change a user's effective
// rights — that round trip is what the one-shot migration relies on.
TEST(AccessRightsForRolesTest, InvertsRolesForUser) {
  for (const std::uint32_t rights : {0u, kControl, kConfigure, kRootRights}) {
    std::vector<NodeId> role_ids;
    for (const WellKnownRole role : RolesForUser(rights, /*anon=*/false)) {
      role_ids.push_back(WellKnownRoleId(role));
    }
    EXPECT_EQ(AccessRightsForRoles(role_ids), rights)
        << "round trip changed rights " << rights;
  }
}

TEST(AccessRightsForRolesTest, MapsEachRoleToItsBit) {
  const auto rights = [](WellKnownRole role) {
    const NodeId ids[] = {WellKnownRoleId(role)};
    return AccessRightsForRoles(ids);
  };

  EXPECT_EQ(rights(WellKnownRole::kOperator), kControl);
  EXPECT_EQ(rights(WellKnownRole::kEngineer), kConfigure);
  EXPECT_EQ(rights(WellKnownRole::kSupervisor), kConfigure);
  EXPECT_EQ(rights(WellKnownRole::kConfigureAdmin), kConfigure);
  EXPECT_EQ(rights(WellKnownRole::kSecurityAdmin), kConfigure);

  // The roles every authenticated session holds carry no coarse right of
  // their own; reading a bit out of them would hand every user Control.
  EXPECT_EQ(rights(WellKnownRole::kAnonymous), 0u);
  EXPECT_EQ(rights(WellKnownRole::kAuthenticatedUser), 0u);
  EXPECT_EQ(rights(WellKnownRole::kObserver), 0u);
}

TEST(AccessRightsForRolesTest, IgnoresCustomAndUnknownRoles) {
  // A custom (group) Role grants through its namespace policy, not through
  // these two bits, so it must contribute neither — silently, because it is a
  // perfectly valid role, not an error.
  const NodeId ids[] = {NodeId{1u, 30 /*ROLE namespace*/},
                        NodeId{999999u, 0}};
  EXPECT_EQ(AccessRightsForRoles(ids), 0u);

  EXPECT_EQ(AccessRightsForRoles({}), 0u);
}

TEST(AccessRightsForRolesTest, CombinesBitsAcrossRoles) {
  const NodeId ids[] = {WellKnownRoleId(WellKnownRole::kOperator),
                        WellKnownRoleId(WellKnownRole::kConfigureAdmin)};
  EXPECT_EQ(AccessRightsForRoles(ids), kRootRights);
}

TEST(UserConfigurationTest, MaskBitsMatchTheSpecPositions) {
  // OPC UA Part 18 §5.2.3 Table 24: NoDelete(0), Disabled(1),
  // NoChangeByUser(2), MustChangePassword(3). The values are on the wire, so a
  // renumbering here silently changes what a stored mask means.
  EXPECT_EQ(static_cast<std::uint32_t>(UserConfiguration::kNoDelete), 1u);
  EXPECT_EQ(static_cast<std::uint32_t>(UserConfiguration::kDisabled), 2u);
  EXPECT_EQ(static_cast<std::uint32_t>(UserConfiguration::kNoChangeByUser), 4u);
  EXPECT_EQ(static_cast<std::uint32_t>(UserConfiguration::kMustChangePassword),
            8u);

  const UserConfiguration mask =
      UserConfiguration::kDisabled | UserConfiguration::kNoDelete;
  EXPECT_TRUE(HasUserConfiguration(mask, UserConfiguration::kDisabled));
  EXPECT_TRUE(HasUserConfiguration(mask, UserConfiguration::kNoDelete));
  EXPECT_FALSE(HasUserConfiguration(mask, UserConfiguration::kNoChangeByUser));
  EXPECT_FALSE(
      HasUserConfiguration(mask, UserConfiguration::kMustChangePassword));

  // An empty mask is a real answer -- an ordinary, enabled, deletable account
  // -- not an absent one.
  EXPECT_FALSE(
      HasUserConfiguration(UserConfiguration::kNone, UserConfiguration::kDisabled));
}

TEST(PasswordOptionsTest, MaskBitsMatchTheSpecPositions) {
  // OPC UA Part 18 §5.2.2 Table 23.
  EXPECT_EQ(
      static_cast<std::uint32_t>(PasswordOptions::kSupportInitialPasswordChange),
      1u);
  EXPECT_EQ(static_cast<std::uint32_t>(PasswordOptions::kSupportDisableUser),
            2u);
  EXPECT_EQ(
      static_cast<std::uint32_t>(PasswordOptions::kSupportDisableDeleteForUser),
      4u);
  EXPECT_EQ(static_cast<std::uint32_t>(PasswordOptions::kSupportNoChangeForUser),
            8u);
  EXPECT_EQ(
      static_cast<std::uint32_t>(PasswordOptions::kSupportDescriptionForUser),
      16u);
  EXPECT_EQ(
      static_cast<std::uint32_t>(PasswordOptions::kRequiresUpperCaseCharacters),
      32u);
  EXPECT_EQ(
      static_cast<std::uint32_t>(PasswordOptions::kRequiresLowerCaseCharacters),
      64u);
  EXPECT_EQ(
      static_cast<std::uint32_t>(PasswordOptions::kRequiresDigitCharacters),
      128u);
  EXPECT_EQ(
      static_cast<std::uint32_t>(PasswordOptions::kRequiresSpecialCharacters),
      256u);
}

TEST(CheckPasswordPolicyTest, LengthBoundsAreOptional) {
  constexpr auto kNoOptions = PasswordOptions::kNone;

  // A non-positive bound means "unconstrained", which is how a server that
  // publishes no minimum or no maximum shows up.
  EXPECT_EQ(CheckPasswordPolicy("", kNoOptions, 0, 0),
            PasswordPolicyViolation::kNone);

  EXPECT_EQ(CheckPasswordPolicy("short", kNoOptions, 8, 0),
            PasswordPolicyViolation::kTooShort);
  EXPECT_EQ(CheckPasswordPolicy("longenough", kNoOptions, 8, 0),
            PasswordPolicyViolation::kNone);

  // The bounds are inclusive: a password of exactly the minimum or maximum
  // length is acceptable.
  EXPECT_EQ(CheckPasswordPolicy("12345678", kNoOptions, 8, 8),
            PasswordPolicyViolation::kNone);
  EXPECT_EQ(CheckPasswordPolicy("123456789", kNoOptions, 8, 8),
            PasswordPolicyViolation::kTooLong);
}

TEST(CheckPasswordPolicyTest, CharacterClassRequirements) {
  EXPECT_EQ(CheckPasswordPolicy(
                "lowercase", PasswordOptions::kRequiresUpperCaseCharacters, 0, 0),
            PasswordPolicyViolation::kMissingUpperCase);
  EXPECT_EQ(CheckPasswordPolicy(
                "UPPERCASE", PasswordOptions::kRequiresLowerCaseCharacters, 0, 0),
            PasswordPolicyViolation::kMissingLowerCase);
  EXPECT_EQ(CheckPasswordPolicy("noDigitsHere",
                                PasswordOptions::kRequiresDigitCharacters, 0, 0),
            PasswordPolicyViolation::kMissingDigit);
  EXPECT_EQ(
      CheckPasswordPolicy("nospecials1",
                          PasswordOptions::kRequiresSpecialCharacters, 0, 0),
      PasswordPolicyViolation::kMissingSpecial);

  const PasswordOptions all_required =
      PasswordOptions::kRequiresUpperCaseCharacters |
      PasswordOptions::kRequiresLowerCaseCharacters |
      PasswordOptions::kRequiresDigitCharacters |
      PasswordOptions::kRequiresSpecialCharacters;
  EXPECT_EQ(CheckPasswordPolicy("Passw0rd!", all_required, 8, 64),
            PasswordPolicyViolation::kNone);

  // A kSupport* bit is a capability advertisement, never a password
  // requirement -- a server that supports disabling users must not thereby
  // reject every password.
  EXPECT_EQ(CheckPasswordPolicy("a", PasswordOptions::kSupportDisableUser, 0, 0),
            PasswordPolicyViolation::kNone);
}

TEST(CheckPasswordPolicyTest, ClassifiesByAsciiByteNotLocale) {
  // Non-ASCII bytes count as "special" rather than as letters: the spec
  // defines the classes over ASCII, and deferring to <cctype> would make the
  // same password pass on one host's locale and fail on another's.
  EXPECT_EQ(CheckPasswordPolicy("\xd0\x9f\xd0\xb0\xd1\x80\xd0\xbe\xd0\xbb\xd1\x8c",
                                PasswordOptions::kRequiresSpecialCharacters, 0,
                                0),
            PasswordPolicyViolation::kNone);
  EXPECT_EQ(CheckPasswordPolicy("\xd0\x9f\xd0\xb0\xd1\x80\xd0\xbe\xd0\xbb\xd1\x8c",
                                PasswordOptions::kRequiresUpperCaseCharacters, 0,
                                0),
            PasswordPolicyViolation::kMissingUpperCase);

  // Length is measured in bytes, which is what a credential store hashes.
  EXPECT_EQ(CheckPasswordPolicy("\xd0\x9f\xd0\xb0", PasswordOptions::kNone, 4, 0),
            PasswordPolicyViolation::kNone);
}

TEST(UserManagementDataTypeTest, IsValueEqualityComparable) {
  const UserManagementDataType user{
      .user_name = "ivanov",
      .user_configuration = UserConfiguration::kDisabled,
      .description = "Dispatcher, north"};
  EXPECT_EQ(user, user);

  UserManagementDataType enabled = user;
  enabled.user_configuration = UserConfiguration::kNone;
  EXPECT_NE(user, enabled);
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
