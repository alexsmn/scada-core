#pragma once

#include "model/namespaces.h"

namespace security {

namespace numeric_id {

const scada::NumericId Users = 29;

}  // namespace numeric_id

namespace id {

const scada::NodeId RootUser{234, NamespaceIndexes::SCADA};
const scada::NodeId Users{numeric_id::Users, NamespaceIndexes::SCADA};
const scada::NodeId UserType_AccessRights{170, NamespaceIndexes::SCADA};
const scada::NodeId UserType{16, NamespaceIndexes::SCADA};
const scada::NodeId UserType_ChangePassword{334, NamespaceIndexes::SCADA};

}  // namespace id

}  // namespace security
