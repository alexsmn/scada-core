#pragma once

#include "core/standard_node_ids.h"
#include "model/namespaces.h"

namespace scada {

constexpr scada::NamespaceIndex kNamespaceIndex = NamespaceIndexes::SCADA;

namespace id {

const scada::NodeId Creates{297, kNamespaceIndex}; // Ref
const scada::NodeId PropertyCategories{310, kNamespaceIndex};
const scada::NodeId HasPropertyCategory{311, kNamespaceIndex};
const scada::NodeId PropertyCategories_General{312, kNamespaceIndex};
const scada::NodeId PropertyCategories_Channels{313, kNamespaceIndex};
const scada::NodeId PropertyCategories_Conversion{314, kNamespaceIndex};
const scada::NodeId PropertyCategories_Filtering{315, kNamespaceIndex};
const scada::NodeId PropertyCategories_Display{316, kNamespaceIndex};
const scada::NodeId PropertyCategories_History{317, kNamespaceIndex};
const scada::NodeId PropertyCategories_Simulation{318, kNamespaceIndex};
const scada::NodeId PropertyCategories_Limits{319, kNamespaceIndex};

const scada::NodeId NextId{328, kNamespaceIndex};

} // namespace id

} // namespace scada