#include "scada/attribute_ids.h"

#include <iterator>
#include <string>

std::string ToString(scada::AttributeId attribute_id) {
  static const char* kStrings[] = {
      "Unknown",         "NodeId",
      "NodeClass",       "BrowseName",
      "DisplayName",     "Description",
      "WriteMask",       "UserWriteMask",
      "IsAbstract",      "Symmetric",
      "InverseName",     "ContainsNoLoops",
      "EventNotifier",   "Value",
      "DataType",        "ValueRank",
      "ArrayDimensions", "AccessLevel",
      "UserAccessLevel", "MinimumSamplingInterval",
      "Historizing",     "Executable",
      "UserExecutable",  "DataTypeDefinition",
      "RolePermissions", "UserRolePermissions",
  };
  static_assert(std::size(kStrings) ==
                static_cast<size_t>(scada::AttributeId::Count));
  // Attribute ids arrive from clients, and OPC UA defines ids past the end of
  // this table (AccessLevelEx and AccessRestrictions, for instance), so an
  // unchecked index read out of bounds whenever one was logged.
  const auto index = static_cast<size_t>(attribute_id);
  if (index >= std::size(kStrings))
    return "AttributeId(" +
           std::to_string(static_cast<long long>(attribute_id)) + ")";
  return kStrings[index];
}
