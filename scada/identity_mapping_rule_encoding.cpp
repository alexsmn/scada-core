#include "scada/identity_mapping_rule_encoding.h"

#include "scada/extension_object.h"
#include "scada/variant.h"

#include <any>
#include <utility>

namespace scada {

Variant EncodeIdentityMappingRules(std::span<const IdentityMappingRule> rules) {
  const ExpandedNodeId data_type_id{
      NodeId{kIdentityMappingRuleTypeDataTypeId, 0}};

  std::vector<ExtensionObject> objects;
  objects.reserve(rules.size());
  for (const IdentityMappingRule& rule : rules) {
    objects.emplace_back(data_type_id, std::any{rule});
  }
  return Variant{std::move(objects)};
}

ExtensionObject MakeIdentityMappingRuleObject(IdentityMappingRule rule) {
  return ExtensionObject{
      ExpandedNodeId{NodeId{kIdentityMappingRuleTypeDataTypeId, 0}},
      std::any{std::move(rule)}};
}

std::optional<IdentityMappingRule> DecodeIdentityMappingRule(
    const ExtensionObject& object) {
  if (const auto* rule = std::any_cast<IdentityMappingRule>(&object.value())) {
    return *rule;
  }
  return std::nullopt;
}

}  // namespace scada
