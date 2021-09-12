#pragma once

#include "core/standard_node_ids.h"
#include "model/namespaces.h"

namespace scada {

namespace id {

const scada::NodeId PropertyCategories{310, NamespaceIndexes::SCADA};
const scada::NodeId Creates{297, NamespaceIndexes::SCADA}; // Ref
const scada::NodeId HasPropertyCategory{311, NamespaceIndexes::SCADA};
const scada::NodeId GeneralPropertyCategory{312, NamespaceIndexes::SCADA};

const scada::NodeId NextId{335, NamespaceIndexes::SCADA};

} // namespace id

}
