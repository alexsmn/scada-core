#include "node_id_util.h"

#include "base/strings/strcat.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_util.h"
#include "model/namespaces.h"

namespace {

scada::NodeId ScadaStringToNumericNodeId(base::StringPiece scada_string) {
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

  unsigned numeric_id = 0;
  if (!base::StringToUint(scada_string.substr(p + 1), &numeric_id))
    return scada::NodeId{};

  return scada::NodeId{numeric_id,
                       static_cast<scada::NamespaceIndex>(namespace_index)};
}

}  // namespace

bool IsNestedNodeId(const scada::NodeId& node_id,
                    scada::NodeId& parent_id,
                    base::StringPiece& nested_name) {
  if (node_id.type() != scada::NodeIdType::String)
    return false;

  const base::StringPiece string_id = node_id.string_id();
  auto p = string_id.find('!');
  if (p == std::string::npos)
    return false;

  parent_id = NodeIdFromScadaString(string_id.substr(0, p));
  if (parent_id.is_null())
    return false;

  nested_name = string_id.substr(p + 1);
  return true;
}

scada::NodeId MakeNestedNodeId(const scada::NodeId& parent_id,
                               base::StringPiece nested_name) {
  assert(!parent_id.is_null());
  assert(!nested_name.empty());
  return scada::NodeId{
      NodeIdToScadaString(parent_id) + '!' + nested_name.as_string(),
      parent_id.namespace_index()};
}

bool GetNestedSubName(const scada::NodeId& node_id,
                      const scada::NodeId& nested_id,
                      base::StringPiece& nested_name) {
  if (node_id.type() != scada::NodeIdType::String)
    return false;

  const base::StringPiece string_id = node_id.string_id();
  const std::string nested_string_id = NodeIdToScadaString(nested_id);

  if (string_id.size() < nested_string_id.size())
    return false;
  if (!base::StartsWith(string_id, nested_string_id,
                        base::CompareCase::SENSITIVE))
    return false;

  if (string_id.size() == nested_string_id.size())
    return true;

  if (string_id.size() < nested_string_id.size() + 1)
    return false;
  if (string_id[nested_string_id.size()] != '!')
    return false;

  nested_name = string_id.substr(nested_string_id.size() + 1);
  return true;
}

std::string NodeIdToScadaString(const scada::NodeId& node_id) {
  switch (node_id.type()) {
    case scada::NodeIdType::Numeric: {
      const auto namespace_name = GetNamespaceName(node_id.namespace_index());
      if (namespace_name.empty()) {
        return base::StrCat({"NS",
                             base::NumberToString(node_id.namespace_index()),
                             ".", base::NumberToString(node_id.numeric_id())});
      } else {
        return base::StrCat(
            {namespace_name, ".", base::NumberToString(node_id.numeric_id())});
      }
    }

    case scada::NodeIdType::String:
      assert(node_id.namespace_index() == 0);
      return node_id.string_id();

    default:
      assert(false);
      return {};
  }
}

scada::NodeId NodeIdFromScadaString(base::StringPiece scada_string) {
  auto p = scada_string.find('!');
  if (p != base::StringPiece::npos)
    return scada::NodeId{scada_string.as_string(), 0};
  else
    return ScadaStringToNumericNodeId(scada_string);
}
