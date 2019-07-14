#pragma once

#include "core/standard_node_ids.h"
#include "model/namespaces.h"

namespace security {

constexpr scada::NamespaceIndex kNamespaceIndex = NamespaceIndexes::SECURITY;

namespace numeric_id {

constexpr scada::NumericId RootUser = 38; // Object
constexpr scada::NumericId RootUser_AccessRights = 39; // Variable
constexpr scada::NumericId UserType = 16; // ObjectType
constexpr scada::NumericId UserType_AccessRights = 17; // Variable
constexpr scada::NumericId Users = 29; // Object

} //  namespace numeric_id

namespace id {

const scada::NodeId RootUser{numeric_id::RootUser, kNamespaceIndex}; // Object
const scada::NodeId RootUser_AccessRights{numeric_id::RootUser_AccessRights, kNamespaceIndex}; // Variable
const scada::NodeId UserType{numeric_id::UserType, kNamespaceIndex}; // ObjectType
const scada::NodeId UserType_AccessRights{numeric_id::UserType_AccessRights, kNamespaceIndex}; // Variable
const scada::NodeId Users{numeric_id::Users, kNamespaceIndex}; // Object

} //  namespace id

} //  namespace security

