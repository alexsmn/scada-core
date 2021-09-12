#pragma once

#include <vector>

#include "core/node_id.h"
#include "core/variant.h"

namespace scada {

typedef std::pair<NodeId /*prop_type_id*/, Variant /*value*/> NodeProperty;
typedef std::vector<NodeProperty> NodeProperties;

inline const Variant* FindProperty(const NodeProperties& properties,
                                   const scada::NodeId& prop_decl_id) {
  auto i = std::find_if(
      properties.begin(), properties.end(),
      [&prop_decl_id](auto& p) { return p.first == prop_decl_id; });
  return i != properties.end() ? &i->second : nullptr;
}

}  // namespace scada
