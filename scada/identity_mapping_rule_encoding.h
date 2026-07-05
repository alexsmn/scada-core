#pragma once

#include "scada/authorization.h"

#include <optional>
#include <span>

namespace scada {

class ExtensionObject;
class Variant;

// OPC UA DataType NodeId of the IdentityMappingRuleType structure (ns=0,
// i=15634) and its DefaultBinary encoding object (i=15736), verified against
// the official 1.05 NodeIds.csv
// (https://files.opcfoundation.org/schemas/UA/1.05/NodeIds.csv).
// OPC UA Part 18 §4.4.3 IdentityMappingRuleType,
// https://reference.opcfoundation.org/Core/Part18/v105/docs/4.4.3
inline constexpr NumericId kIdentityMappingRuleTypeDataTypeId = 15634;
inline constexpr NumericId kIdentityMappingRuleTypeDefaultBinaryId = 15736;

// Encodes rules as an OPC UA `IdentityMappingRuleType[]` attribute value — an
// array Variant of ExtensionObjects, each wrapping one rule with the
// IdentityMappingRuleType DataType id. Used to serve a Role's Identities
// property (OPC UA Part 18 §4.4.1). An empty input yields an empty array.
Variant EncodeIdentityMappingRules(std::span<const IdentityMappingRule> rules);

// Wraps a single rule as an ExtensionObject (e.g. an AddIdentity /
// RemoveIdentity method argument, OPC UA Part 18 §4.4.5/§4.4.6).
ExtensionObject MakeIdentityMappingRuleObject(IdentityMappingRule rule);

// Extracts the rule from an ExtensionObject payload (a decoded method
// argument), or nullopt when the payload is not an IdentityMappingRule.
std::optional<IdentityMappingRule> DecodeIdentityMappingRule(
    const ExtensionObject& object);

}  // namespace scada
