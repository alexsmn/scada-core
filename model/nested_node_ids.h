#pragma once

#include "model/node_id_util.h"

#include <optional>

struct NestedNodeId {
  scada::NodeId parent_id;
  std::string_view nested_name;
};

inline std::optional<NestedNodeId> ParseNestedNodeId(
    const scada::NodeId& node_id) {
  scada::NodeId parent_id;
  std::string_view nested_name;
  return IsNestedNodeId(node_id, parent_id, nested_name)
             ? std::optional<NestedNodeId>{NestedNodeId{parent_id, nested_name}}
             : std::optional<NestedNodeId>{};
}