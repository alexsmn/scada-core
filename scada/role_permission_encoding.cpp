#include "scada/role_permission_encoding.h"

#include "scada/extension_object.h"
#include "scada/variant.h"

#include <any>
#include <utility>

namespace scada {

Variant EncodeRolePermissions(
    const std::vector<RolePermissionType>& entries) {
  const ExpandedNodeId data_type_id{NodeId{kRolePermissionTypeDataTypeId, 0}};

  std::vector<ExtensionObject> objects;
  objects.reserve(entries.size());
  for (const RolePermissionType& entry : entries) {
    objects.emplace_back(data_type_id, std::any{entry});
  }
  return Variant{std::move(objects)};
}

std::optional<std::vector<RolePermissionType>> DecodeRolePermissions(
    const Variant& value) {
  if (!value.is_array() || value.type() != Variant::EXTENSION_OBJECT) {
    return std::nullopt;
  }

  const auto& objects = value.get<std::vector<ExtensionObject>>();
  std::vector<RolePermissionType> entries;
  entries.reserve(objects.size());
  for (const ExtensionObject& object : objects) {
    // A foreign payload is dropped rather than defaulted: a zero-filled entry
    // would read as a real Role granting nothing, which is a claim about
    // authorization the decoder has no basis for.
    if (auto entry = DecodeRolePermissionObject(object)) {
      entries.push_back(*std::move(entry));
    }
  }
  return entries;
}

std::optional<RolePermissionType> DecodeRolePermissionObject(
    const ExtensionObject& object) {
  if (const auto* entry = std::any_cast<RolePermissionType>(&object.value())) {
    return *entry;
  }
  return std::nullopt;
}

}  // namespace scada
