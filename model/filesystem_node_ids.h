#pragma once

#include "core/standard_node_ids.h"
#include "model/namespaces.h"

namespace filesystem {

constexpr scada::NamespaceIndex kNamespaceIndex = NamespaceIndexes::FILESYSTEM;

namespace numeric_id {

constexpr scada::NumericId FileDirectoryType = 15017; // ObjectType
constexpr scada::NumericId FileSystem = 15016; // Object
constexpr scada::NumericId FileType = 15018; // VariableType
constexpr scada::NumericId FileType_LastUpdateTime = 15019; // Variable
constexpr scada::NumericId FileType_Size = 15020; // Variable

} //  namespace numeric_id

namespace id {

const scada::NodeId FileDirectoryType{numeric_id::FileDirectoryType, kNamespaceIndex}; // ObjectType
const scada::NodeId FileSystem{numeric_id::FileSystem, kNamespaceIndex}; // Object
const scada::NodeId FileType{numeric_id::FileType, kNamespaceIndex}; // VariableType
const scada::NodeId FileType_LastUpdateTime{numeric_id::FileType_LastUpdateTime, kNamespaceIndex}; // Variable
const scada::NodeId FileType_Size{numeric_id::FileType_Size, kNamespaceIndex}; // Variable

} //  namespace id

} //  namespace filesystem

