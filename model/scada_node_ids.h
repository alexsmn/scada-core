#pragma once

#include "core/standard_node_ids.h"
#include "model/namespaces.h"

namespace scada {

constexpr scada::NamespaceIndex kNamespaceIndex = NamespaceIndexes::SCADA;

namespace numeric_id {

constexpr scada::NumericId Creates = 23; // ReferenceType
constexpr scada::NumericId GeneralPropertyCategory = 15001; // Object
constexpr scada::NumericId HasPropertyCategory = 15028; // ReferenceType
constexpr scada::NumericId PropertyCategories = 15029; // Object

} //  namespace numeric_id

namespace id {

const scada::NodeId Creates{numeric_id::Creates, kNamespaceIndex}; // ReferenceType
const scada::NodeId GeneralPropertyCategory{numeric_id::GeneralPropertyCategory, kNamespaceIndex}; // Object
const scada::NodeId HasPropertyCategory{numeric_id::HasPropertyCategory, kNamespaceIndex}; // ReferenceType
const scada::NodeId PropertyCategories{numeric_id::PropertyCategories, kNamespaceIndex}; // Object

} //  namespace id

} //  namespace scada

