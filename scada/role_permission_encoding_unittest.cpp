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

}  // namespace
}  // namespace scada
