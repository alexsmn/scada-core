#pragma once

#include "model/scada_node_ids.h"

namespace filesystem {

constexpr scada::NamespaceIndex kNamespaceIndex = NamespaceIndexes::SCADA;

namespace id {

const scada::NodeId FileSystem{304, kNamespaceIndex};
const scada::NodeId FileDirectoryType{305, kNamespaceIndex};
const scada::NodeId FileType{306, kNamespaceIndex};
const scada::NodeId FileType_LastUpdateTime{307, kNamespaceIndex};
const scada::NodeId FileType_Size{308, kNamespaceIndex};

}  // namespace id

}  // namespace filesystem