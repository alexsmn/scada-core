#pragma once

#include "scada/standard_node_ids.h"
#include "model/namespaces.h"

namespace scada {

namespace id {

const scada::NodeId PropertyCategories{310, NamespaceIndexes::SCADA};
const scada::NodeId Creates{297, NamespaceIndexes::SCADA}; // Ref
const scada::NodeId HasParentReferenceType{340, NamespaceIndexes::SCADA};  // Ref
const scada::NodeId HasPropertyCategory{311, NamespaceIndexes::SCADA};
const scada::NodeId GeneralPropertyCategory{312, NamespaceIndexes::SCADA};
const scada::NodeId MetricType{336, NamespaceIndexes::SCADA};
const scada::NodeId MetricType_AggregateFunction{337, NamespaceIndexes::SCADA};
const scada::NodeId MetricType_AggregateInterval{338, NamespaceIndexes::SCADA};

const scada::NodeId NextId{352, NamespaceIndexes::SCADA};

} // namespace id

}
