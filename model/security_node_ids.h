#pragma once

#include "model/scada_node_ids.h"

namespace security {

constexpr scada::NamespaceIndex kNamespaceIndex = NamespaceIndexes::SCADA;

namespace numeric_id {

const scada::NumericId Users = 29;

}

namespace id {

const scada::NodeId Users{numeric_id::Users, kNamespaceIndex};
const scada::NodeId RootUser{234, kNamespaceIndex};
const scada::NodeId UserType_AccessRights{170, kNamespaceIndex};
const scada::NodeId UserType{16, kNamespaceIndex};

}  // namespace id

}  // namespace security