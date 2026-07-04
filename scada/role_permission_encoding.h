#pragma once

#include "scada/authorization.h"

#include <vector>

namespace scada {

class Variant;

// OPC UA DataType NodeId of the RolePermissionType structure (namespace 0,
// i=96), used as the ExtensionObject type id when encoding RolePermissions /
// UserRolePermissions attribute values.
// https://reference.opcfoundation.org/Core/Part3/v105/docs/8.56
inline constexpr NumericId kRolePermissionTypeDataTypeId = 96;

// Encodes RolePermissionType entries as an OPC UA `RolePermissionType[]`
// attribute value: an array Variant of ExtensionObjects, each wrapping one
// entry with the RolePermissionType DataType id. Used to serve the
// RolePermissions and UserRolePermissions attributes (OPC UA Part 3
// §5.2.9/§5.2.10). An empty input yields an empty array value.
Variant EncodeRolePermissions(const std::vector<RolePermissionType>& entries);

}  // namespace scada
