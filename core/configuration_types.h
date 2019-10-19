#pragma once

#include <vector>

#include "core/attribute_service.h"
#include "core/node_id.h"
#include "core/variant.h"

namespace scada {

typedef std::pair<NodeId /*prop_type_id*/, Variant /*value*/> NodeProperty;
typedef std::vector<NodeProperty> NodeProperties;

inline bool operator==(const ReadValueId& a, const ReadValueId& b) {
  return std::tie(a.node_id, a.attribute_id) ==
         std::tie(b.node_id, b.attribute_id);
}

}  // namespace scada
