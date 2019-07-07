#pragma once

#include "model/scada_node_ids.h"

namespace security {

namespace numeric_id {

const scada::NumericId Users = 29;

}

namespace id {

const scada::NodeId Users{numeric_id::Users, NamespaceIndexes::SCADA};
const scada::NodeId RootUser{234, NamespaceIndexes::SCADA};
const scada::NodeId UserType_AccessRights{170, NamespaceIndexes::SCADA};
const scada::NodeId UserType{16, NamespaceIndexes::SCADA};

}  // namespace id

}  // namespace security