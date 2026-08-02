#pragma once

#include "scada/authorization.h"

#include <optional>
#include <vector>

namespace scada {

class ExtensionObject;
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

// Extracts the entries from a `RolePermissionType[]` Variant produced by
// `EncodeRolePermissions` — the read path for the RolePermissions and
// UserRolePermissions attributes (OPC UA Part 3 §5.2.9/§5.2.10).
//
// This is how a CLIENT learns what each Role grants. It must not carry its own
// copy of the server's role -> permission map: a stale copy would let it tell
// an operator that an account may do something the server will refuse.
//
// Returns nullopt when the Variant is not an ExtensionObject array, and skips
// entries whose payload is not a RolePermissionType — a foreign entry must not
// silently become a grant. An empty array is a real answer (a node that grants
// nothing to anybody) and decodes to an empty vector, not to nullopt.
std::optional<std::vector<RolePermissionType>> DecodeRolePermissions(
    const Variant& value);

// Extracts one entry from an ExtensionObject payload, or nullopt when the
// payload is not a RolePermissionType.
std::optional<RolePermissionType> DecodeRolePermissionObject(
    const ExtensionObject& object);

}  // namespace scada
