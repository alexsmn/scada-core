#pragma once

#include "scada/node_id.h"
#include "model/namespaces.h"

namespace filesystem {

namespace id {

const scada::NodeId FileSystem{304, NamespaceIndexes::SCADA};
const scada::NodeId FileDirectoryType{305, NamespaceIndexes::SCADA};
const scada::NodeId FileType{306, NamespaceIndexes::SCADA};
const scada::NodeId FileType_LastUpdateTime{307, NamespaceIndexes::SCADA};
const scada::NodeId FileType_Size{308, NamespaceIndexes::SCADA};

}  // namespace id

}  // namespace filesystem
