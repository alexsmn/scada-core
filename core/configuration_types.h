#pragma once

#include <vector>

#include "core/attribute_ids.h"
#include "core/node_id.h"
#include "core/status.h"
#include "core/variant.h"
#include "core/write_flags.h"

namespace scada {

typedef std::pair<NodeId /*prop_type_id*/, Variant /*value*/> NodeProperty;
typedef std::vector<NodeProperty> NodeProperties;

struct ReadValueId {
  NodeId node_id;
  AttributeId attribute_id;
};

inline bool operator==(const ReadValueId& a, const ReadValueId& b) {
  return std::tie(a.node_id, a.attribute_id) ==
         std::tie(b.node_id, b.attribute_id);
}

}  // namespace scada

inline std::ostream& operator<<(std::ostream& stream,
                                const scada::ReadValueId& v) {
  return stream << "{" << v.node_id << ", " << v.attribute_id << "}";
}