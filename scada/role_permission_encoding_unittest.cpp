#include "scada/role_permission_encoding.h"

#include "scada/extension_object.h"
#include "scada/variant.h"

#include <any>

#include <gtest/gtest.h>

namespace scada {
namespace {

TEST(RolePermissionEncodingTest, EncodesEntriesAsExtensionObjectArray) {
  const std::vector<RolePermissionType> entries = UserRolePermissions(
      /*access_rights=*/0, /*is_anonymous=*/true);
  ASSERT_EQ(entries.size(), 1u);  // Anonymous role only.

  const Variant value = EncodeRolePermissions(entries);
  ASSERT_TRUE(value.is_array());
  ASSERT_EQ(value.type(), Variant::EXTENSION_OBJECT);

  const auto& objects = value.get<std::vector<ExtensionObject>>();
  ASSERT_EQ(objects.size(), 1u);
  EXPECT_EQ(objects.front().data_type_id().node_id(),
            (NodeId{kRolePermissionTypeDataTypeId, 0}));

  const auto* decoded =
      std::any_cast<RolePermissionType>(&objects.front().value());
  ASSERT_NE(decoded, nullptr);
  EXPECT_EQ(decoded->role_id, WellKnownRoleId(WellKnownRole::kAnonymous));
  EXPECT_EQ(decoded->permissions, DefaultPermissionsForRole(
                                      WellKnownRole::kAnonymous));
}

TEST(RolePermissionEncodingTest, EmptyEntriesYieldEmptyArray) {
  const Variant value = EncodeRolePermissions({});
  EXPECT_TRUE(value.is_array());
  EXPECT_TRUE(value.get<std::vector<ExtensionObject>>().empty());
}

// The round trip a client depends on: it reads the server's whole role map off
// the RolePermissions attribute instead of carrying its own copy.
TEST(RolePermissionEncodingTest, DecodeRestoresTheServerRoleMap) {
  const std::vector<RolePermissionType> entries = DefaultRolePermissions();
  ASSERT_EQ(entries.size(), 8u);

  const auto decoded = DecodeRolePermissions(EncodeRolePermissions(entries));
  ASSERT_TRUE(decoded);
  EXPECT_EQ(*decoded, entries);
}

// An empty map is a real answer — a node granting nothing to anybody — and
// must stay distinguishable from a value that could not be decoded at all.
TEST(RolePermissionEncodingTest, EmptyArrayDecodesToEmptyNotNullopt) {
  const auto decoded = DecodeRolePermissions(EncodeRolePermissions({}));
  ASSERT_TRUE(decoded);
  EXPECT_TRUE(decoded->empty());
}

TEST(RolePermissionEncodingTest, NonExtensionObjectValueDecodesToNullopt) {
  EXPECT_FALSE(DecodeRolePermissions(Variant{Int32{42}}));
  EXPECT_FALSE(DecodeRolePermissions(Variant{}));
}

// A foreign payload is dropped, not defaulted: a zero-filled entry would read
// as a real Role that grants nothing.
TEST(RolePermissionEncodingTest, ForeignPayloadsAreDropped) {
  std::vector<ExtensionObject> objects;
  objects.emplace_back(ExpandedNodeId{NodeId{kRolePermissionTypeDataTypeId, 0}},
                       std::any{RolePermissionType{
                           .role_id = WellKnownRoleId(WellKnownRole::kOperator),
                           .permissions = Permission::kRead}});
  objects.emplace_back(ExpandedNodeId{NodeId{kRolePermissionTypeDataTypeId, 0}},
                       std::any{std::string{"not a role permission"}});

  const auto decoded = DecodeRolePermissions(Variant{std::move(objects)});
  ASSERT_TRUE(decoded);
  ASSERT_EQ(decoded->size(), 1u);
  EXPECT_EQ(decoded->front().role_id,
            WellKnownRoleId(WellKnownRole::kOperator));
}

}  // namespace
}  // namespace scada
