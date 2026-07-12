#pragma once

#include "scada/node_id.h"
#include "model/namespaces.h"

#include "model/model_compat.h"
namespace scada::opc {

namespace id {

// TODO: Move to OPC module.
const scada::NodeId OPC{329, NamespaceIndexes::SCADA};

} // namespace id

}
