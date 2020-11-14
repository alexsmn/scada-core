#include "node_id_util.h"

#include "base/string_piece_util.h"
#include "base/strings/strcat.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_util.h"
#include "base/strings/stringprintf.h"
#include "model/namespaces.h"

namespace {

scada::NodeId ScadaStringToNumericNodeId(std::string_view scada_string) {
  auto p = scada_string.find('.');
  if (p == std::string_view::npos)
    return scada::NodeId{};

  int namespace_index = FindNamespaceIndexByName(scada_string.substr(0, p));
  if (namespace_index == -1)
    return scada::NodeId{};

  unsigned numeric_id = 0;
  if (!base::StringToUint(ToStringPiece(scada_string.substr(p + 1)),
                          &numeric_id))
    return scada::NodeId{};

  return scada::NodeId{numeric_id,
                       static_cast<scada::NamespaceIndex>(namespace_index)};
}

}  // namespace

bool IsNestedNodeId(const scada::NodeId& node_id,
                    scada::NodeId& parent_id,
                    std::string_view& nested_name) {
  if (node_id.type() != scada::NodeIdType::String)
    return false;

  const std::string_view string_id = node_id.string_id();
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
                               std::string_view nested_name) {
  assert(!nested_name.empty());
  return scada::NodeId{
      NodeIdToScadaString(parent_id) + '!' + std::string{nested_name}, 0};
}

bool GetNestedSubName(const scada::NodeId& node_id,
                      const scada::NodeId& nested_id,
                      std::string_view& nested_name) {
  if (node_id.type() != scada::NodeIdType::String)
    return false;

  const std::string_view string_id = node_id.string_id();
  const std::string nested_string_id = NodeIdToScadaString(nested_id);

  if (string_id.size() < nested_string_id.size())
    return false;
  if (!base::StartsWith(ToStringPiece(string_id), nested_string_id,
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
  std::string result;

  switch (node_id.type()) {
    case scada::NodeIdType::Numeric:
      result = base::StrCat(
          {ToStringPiece(GetNamespaceName(node_id.namespace_index())), ".",
           base::IntToString(node_id.numeric_id())});
      break;

    case scada::NodeIdType::String:
      assert(node_id.namespace_index() == 0);
      result = node_id.string_id();
      break;

    default:
      assert(false);
  }

  return result;
}

scada::NodeId NodeIdFromScadaString(std::string_view scada_string) {
  auto p = scada_string.find('!');
  if (p != std::string_view::npos)
    return scada::NodeId{std::string{scada_string}, 0};
  else
    return ScadaStringToNumericNodeId(scada_string);
}
