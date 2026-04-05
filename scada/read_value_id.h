#pragma once

#include "scada/attribute_ids.h"
#include "scada/node_id.h"

namespace scada {

// The structure is used by attribute and monitored item services.
struct ReadValueId {
  NodeId node_id;
  AttributeId attribute_id = scada::AttributeId::Value;

  bool operator==(const ReadValueId&) const = default;
};

inline std::ostream& operator<<(std::ostream& stream, const ReadValueId& v) {
  return stream << "{" << v.node_id << ", " << v.attribute_id << "}";
}

}  // namespace scada
