#include "node_id_util.h"

#include "base/strings/strcat.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_util.h"
#include "model/namespaces.h"

bool IsNestedNodeId(const scada::NodeId& node_id,
                    scada::NodeId& parent_id,
                    base::StringPiece& nested_name) {
  if (node_id.type() != scada::NodeIdType::String)
    return false;

  const base::StringPiece string_id = node_id.string_id();
  auto p = string_id.find('!');
  if (p == std::string::npos)
    return false;

  // Parent id can only be a number for now.
  scada::NumericId parent_numeric_id = 0;
  if (!base::StringToUint(string_id.substr(0, p), &parent_numeric_id))
    return false;

  parent_id = scada::NodeId{parent_numeric_id, node_id.namespace_index()};
  nested_name = string_id.substr(p + 1);
  return true;
}

scada::NodeId MakeNestedNodeId(const scada::NodeId& parent_id,
                               base::StringPiece nested_name) {
  assert(!parent_id.is_null());
  assert(!nested_name.empty());

  auto parent_identifier = parent_id.type() == scada::NodeIdType::Numeric
                               ? base::NumberToString(parent_id.numeric_id())
                               : parent_id.string_id();
  auto identifier = base::StrCat({parent_identifier, "!", nested_name});

  return scada::NodeId{std::move(identifier), parent_id.namespace_index()};
}

std::string NodeIdToScadaString(const scada::NodeId& node_id) {
  std::string namespace_name =
      GetNamespaceName(node_id.namespace_index()).as_string();
  if (namespace_name.empty()) {
    namespace_name =
        base::StrCat({"NS", base::NumberToString(node_id.namespace_index())});
  }

  std::string identifier;

  switch (node_id.type()) {
    case scada::NodeIdType::Numeric:
      identifier = base::NumberToString(node_id.numeric_id());
      break;

    case scada::NodeIdType::String:
      identifier = node_id.string_id();
      break;

    default:
      assert(false);
      return {};
  }

  return base::StrCat({namespace_name, ".", identifier});
}

scada::NodeId NodeIdFromScadaString(base::StringPiece scada_string) {
  const auto p = scada_string.find('.');
  if (p == base::StringPiece::npos)
    return scada::NodeId{};

  const auto namespace_name = scada_string.substr(0, p);
  int namespace_index = FindNamespaceIndexByName(namespace_name);
  if (namespace_index == -1) {
    if (!namespace_name.starts_with("NS"))
      return scada::NodeId{};
    if (!base::StringToInt(namespace_name.substr(2), &namespace_index))
      return scada::NodeId{};
  }

  const auto& identifier = scada_string.substr(p + 1);

  if (identifier.find('!') != base::StringPiece::npos) {
    return scada::NodeId{identifier.as_string(),
                         static_cast<scada::NamespaceIndex>(namespace_index)};
  }

  scada::NumericId numeric_id = 0;
  if (!base::StringToUint(identifier, &numeric_id)) {
    return scada::NodeId{identifier.as_string(),
                         static_cast<scada::NamespaceIndex>(namespace_index)};
  }

  return scada::NodeId{numeric_id,
                       static_cast<scada::NamespaceIndex>(namespace_index)};
}
