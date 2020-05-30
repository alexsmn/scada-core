#pragma once

#include <vector>

#include "core/node_id.h"
#include "core/variant.h"

namespace scada {

typedef std::pair<NodeId /*prop_type_id*/, Variant /*value*/> NodeProperty;
typedef std::vector<NodeProperty> NodeProperties;

}  // namespace scada
