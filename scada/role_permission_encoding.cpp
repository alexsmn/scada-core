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

}  // namespace scada
