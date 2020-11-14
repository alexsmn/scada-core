#pragma once

#include "core/node_id.h"

#include <string_view>

bool IsNestedNodeId(const scada::NodeId& node_id,
                    scada::NodeId& parent_id,
                    std::string_view& nested_name);
scada::NodeId MakeNestedNodeId(const scada::NodeId& parent_id,
                               std::string_view nested_name);
bool GetNestedSubName(const scada::NodeId& node_id,
                      const scada::NodeId& nested_id,
                      std::string_view& nested_name);

scada::NodeId NodeIdFromScadaString(std::string_view scada_string);
std::string NodeIdToScadaString(const scada::NodeId& node_id);