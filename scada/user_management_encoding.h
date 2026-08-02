#pragma once

#include "scada/authorization.h"

#include <optional>
#include <span>
#include <vector>

namespace scada {

class ExtensionObject;
class Variant;

// OPC UA DataType NodeId of the UserManagementDataType structure (ns=0,
// i=24281) and its DefaultBinary encoding object (i=24292), verified against
// the official 1.05 NodeIds.csv
// (https://files.opcfoundation.org/schemas/UA/1.05/NodeIds.csv).
// OPC UA Part 18 §5.2.4 UserManagementDataType,
// https://reference.opcfoundation.org/Core/Part18/v105/docs/5.2.4
inline constexpr NumericId kUserManagementDataTypeId = 24281;
inline constexpr NumericId kUserManagementDataTypeDefaultBinaryId = 24292;

// Encodes users as an OPC UA `UserManagementDataType[]` attribute value — an
// array Variant of ExtensionObjects, each wrapping one user with the
// UserManagementDataType DataType id. This is the whole read path for the
// UserManagement object's Users property (OPC UA Part 18 §5.2.2): one Read
// returns every account, so a client's users grid needs no per-user Browse.
// An empty input yields an empty array, which is a server with no accounts —
// distinct from a failed read, which carries a bad StatusCode instead.
Variant EncodeUserManagementUsers(std::span<const UserManagementDataType> users);

// Extracts the users from a `UserManagementDataType[]` Variant produced by
// `EncodeUserManagementUsers`. Returns nullopt when the Variant is not an
// ExtensionObject array, and skips entries whose payload is not a
// UserManagementDataType — a foreign entry must not silently become a real
// account.
std::optional<std::vector<UserManagementDataType>> DecodeUserManagementUsers(
    const Variant& value);

// Wraps a single user as an ExtensionObject.
ExtensionObject MakeUserManagementObject(UserManagementDataType user);

// Extracts the user from an ExtensionObject payload, or nullopt when the
// payload is not a UserManagementDataType.
std::optional<UserManagementDataType> DecodeUserManagementObject(
    const ExtensionObject& object);

}  // namespace scada
