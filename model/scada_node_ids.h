#pragma once

#include "core/standard_node_ids.h"
#include "model/namespaces.h"

namespace id {

const scada::NodeId Creates{297, NamespaceIndexes::SCADA}; // Ref
const scada::NodeId PropertyCategories{310, NamespaceIndexes::SCADA};
const scada::NodeId HasPropertyCategory{311, NamespaceIndexes::SCADA};
const scada::NodeId PropertyCategories_General{312, NamespaceIndexes::SCADA};
const scada::NodeId PropertyCategories_Channels{313, NamespaceIndexes::SCADA};
const scada::NodeId PropertyCategories_Conversion{314, NamespaceIndexes::SCADA};
const scada::NodeId PropertyCategories_Filtering{315, NamespaceIndexes::SCADA};
const scada::NodeId PropertyCategories_Display{316, NamespaceIndexes::SCADA};
const scada::NodeId PropertyCategories_History{317, NamespaceIndexes::SCADA};
const scada::NodeId PropertyCategories_Simulation{318, NamespaceIndexes::SCADA};
const scada::NodeId PropertyCategories_Limits{319, NamespaceIndexes::SCADA};

const scada::NodeId NextId{328, NamespaceIndexes::SCADA};

} // namespace id
