#include "scada/user_management_encoding.h"

#include "scada/extension_object.h"
#include "scada/variant.h"

#include <any>
#include <utility>

namespace scada {

Variant EncodeUserManagementUsers(
    std::span<const UserManagementDataType> users) {
  const ExpandedNodeId data_type_id{NodeId{kUserManagementDataTypeId, 0}};

  std::vector<ExtensionObject> objects;
  objects.reserve(users.size());
  for (const UserManagementDataType& user : users) {
    objects.emplace_back(data_type_id, std::any{user});
  }
  return Variant{std::move(objects)};
}

std::optional<std::vector<UserManagementDataType>> DecodeUserManagementUsers(
    const Variant& value) {
  if (!value.is_array() || value.type() != Variant::EXTENSION_OBJECT) {
    return std::nullopt;
  }

  const auto& objects = value.get<std::vector<ExtensionObject>>();
  std::vector<UserManagementDataType> users;
  users.reserve(objects.size());
  for (const ExtensionObject& object : objects) {
    // An entry whose payload is not a UserManagementDataType is dropped rather
    // than defaulted: a zero-filled account would read as a real, enabled user
    // with an empty name, which is exactly the kind of invented fact a users
    // grid must never show.
    if (auto user = DecodeUserManagementObject(object)) {
      users.push_back(*std::move(user));
    }
  }
  return users;
}

ExtensionObject MakeUserManagementObject(UserManagementDataType user) {
  return ExtensionObject{ExpandedNodeId{NodeId{kUserManagementDataTypeId, 0}},
                         std::any{std::move(user)}};
}

std::optional<UserManagementDataType> DecodeUserManagementObject(
    const ExtensionObject& object) {
  if (const auto* user =
          std::any_cast<UserManagementDataType>(&object.value())) {
    return *user;
  }
  return std::nullopt;
}

}  // namespace scada
