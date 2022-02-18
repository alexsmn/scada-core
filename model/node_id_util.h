#pragma once

#include "base/strings/string_piece.h"
#include "core/node_id.h"

bool IsNestedNodeId(const scada::NodeId& node_id,
                    scada::NodeId& parent_id,
                    std::string_view& nested_name);

inline bool IsNestedNodeId(const scada::NodeId& node_id) {
  scada::NodeId parent_id;
  std::string_view nested_name;
  return IsNestedNodeId(node_id, parent_id, nested_name);
}

scada::NodeId MakeNestedNodeId(const scada::NodeId& parent_id,
                               std::string_view nested_name);
scada::NodeId MakeNestedNodeId(const scada::NodeId& parent_id,
                               std::string_view nested_name,
                               scada::NamespaceIndex namespace_index);
bool GetNestedSubName(const scada::NodeId& node_id,
                      const scada::NodeId& nested_id,
                      std::string_view& nested_name);

scada::NodeId NodeIdFromScadaString(std::string_view scada_string);
std::string NodeIdToScadaString(const scada::NodeId& node_id);
