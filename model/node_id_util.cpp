#include "node_id_util.h"

#include "base/string_piece_util.h"
#include "base/strings/strcat.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_util.h"
#include "model/data_items_node_ids.h"
#include "model/namespaces.h"

namespace {

static std::pair<std::string_view, scada::NodeId> kPredefinedScadaStrings[] = {
    {"Server!CPU", data_items::id::Statistics_TotalCPUUsage},
    {"Server!Mem", data_items::id::Statistics_TotalMemoryUsage},
    {"Server!PCPU", data_items::id::Statistics_ServerCPUUsage},
    {"Server!PMem", data_items::id::Statistics_ServerMemoryUsage},
};

scada::NodeId NodeIdFromPredefinedScadaString(std::string_view scada_string) {
  auto i = std::find_if(
      std::cbegin(kPredefinedScadaStrings), std::cend(kPredefinedScadaStrings),
      [scada_string](const auto& p) { return p.first == scada_string; });
  return i != std::cend(kPredefinedScadaStrings) ? i->second : scada::NodeId{};
}

std::string_view NodeIdToPredefinedScadaString(const scada::NodeId& node_id) {
  auto i = std::find_if(
      std::cbegin(kPredefinedScadaStrings), std::cend(kPredefinedScadaStrings),
      [node_id](const auto& p) { return p.second == node_id; });
  return i != std::cend(kPredefinedScadaStrings) ? i->first
                                                 : std::string_view{};
}

}  // namespace

bool IsNestedNodeId(const scada::NodeId& node_id,
                    scada::NodeId& parent_id,
                    std::string_view& nested_name) {
  if (node_id.type() != scada::NodeIdType::String)
    return false;

  const std::string_view string_id = node_id.string_id();
  const auto p = string_id.find_last_of('!');
  if (p == std::string::npos)
    return false;

  const bool is_top_parent = string_id.find('!') == p;
  std::string_view parent_string_id = string_id.substr(0, p);
  auto parent_namespace_index = node_id.namespace_index();

  // Parent can have optional namespace specification.
  if (auto np = parent_string_id.find(':'); np != parent_string_id.npos) {
    int pni = FindNamespaceIndexByName(parent_string_id.substr(0, np));
    if (pni == -1)
      return false;
    parent_namespace_index = static_cast<scada::NamespaceIndex>(pni);
    parent_string_id = parent_string_id.substr(np + 1);
  }

  // Top parent id can only be a number with optional namespace for now. Next
  // levels are always strings, since they have to contain the `!` separator.
  if (is_top_parent) {
    scada::NumericId parent_numeric_id = 0;
    if (!base::StringToUint(AsStringPiece(parent_string_id),
                            &parent_numeric_id)) {
      return false;
    }
    parent_id = scada::NodeId{parent_numeric_id, parent_namespace_index};
  } else {
    parent_id =
        scada::NodeId{scada::String{parent_string_id}, parent_namespace_index};
  }

  nested_name = string_id.substr(p + 1);
  return true;
}

bool GetRootNestedNodeId(const scada::NodeId& node_id,
                         scada::NodeId& parent_id,
                         std::string_view& nested_name) {
  if (!IsNestedNodeId(node_id, parent_id, nested_name))
    return false;

  scada::NodeId next_parent_id;
  std::string_view next_nested_name;
  while (IsNestedNodeId(parent_id, next_parent_id, next_nested_name)) {
    parent_id = std::move(next_parent_id);
    nested_name = next_nested_name;
  }

  return true;
}

scada::NodeId MakeNestedNodeId(const scada::NodeId& parent_id,
                               std::string_view nested_name) {
  return MakeNestedNodeId(parent_id, nested_name, parent_id.namespace_index());
}

scada::NodeId MakeNestedNodeId(const scada::NodeId& parent_id,
                               std::string_view nested_name,
                               scada::NamespaceIndex namespace_index) {
  assert(!parent_id.is_null());
  assert(!nested_name.empty());

  auto parent_namespace =
      namespace_index != parent_id.namespace_index()
          ? base::NumberToString(parent_id.namespace_index()) + ':'
          : std::string{};
  auto parent_identifier = parent_id.type() == scada::NodeIdType::Numeric
                               ? base::NumberToString(parent_id.numeric_id())
                               : parent_id.string_id();
  auto identifier = base::StrCat(
      {parent_namespace, parent_identifier, "!", AsStringPiece(nested_name)});

  return scada::NodeId{std::move(identifier), namespace_index};
}

bool GetNestedSubName(const scada::NodeId& node_id,
                      const scada::NodeId& nested_id,
                      std::string_view& nested_name) {
  // For backward compatibility.
  if (node_id == nested_id) {
    nested_name = {};
    return true;
  }

  scada::NodeId parent_id;
  if (!IsNestedNodeId(node_id, parent_id, nested_name))
    return false;

  return parent_id == nested_id;
}

std::string NodeIdToScadaString(const scada::NodeId& node_id) {
  if (auto predefined_scada_string = NodeIdToPredefinedScadaString(node_id);
      !predefined_scada_string.empty())
    return std::string{predefined_scada_string};

  std::string namespace_name =
      std::string{GetNamespaceName(node_id.namespace_index())};
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

scada::NodeId NodeIdFromScadaString(std::string_view scada_string) {
  if (auto node_id = NodeIdFromPredefinedScadaString(scada_string);
      !node_id.is_null())
    return node_id;

  const auto p = scada_string.find('.');
  if (p == std::string_view::npos)
    return scada::NodeId{};

  const auto namespace_name = scada_string.substr(0, p);
  int namespace_index = FindNamespaceIndexByName(namespace_name);
  if (namespace_index == -1) {
    if (!base::StartsWith(AsStringPiece(namespace_name), "NS",
                          base::CompareCase::INSENSITIVE_ASCII)) {
      return scada::NodeId{};
    }
    if (!base::StringToInt(AsStringPiece(namespace_name.substr(2)),
                           &namespace_index)) {
      return scada::NodeId{};
    }
  }

  const auto& identifier = scada_string.substr(p + 1);

  if (identifier.find('!') != std::string_view::npos) {
    return scada::NodeId{std::string{identifier},
                         static_cast<scada::NamespaceIndex>(namespace_index)};
  }

  scada::NumericId numeric_id = 0;
  if (!base::StringToUint(AsStringPiece(identifier), &numeric_id)) {
    return scada::NodeId{std::string{identifier},
                         static_cast<scada::NamespaceIndex>(namespace_index)};
  }

  return scada::NodeId{numeric_id,
                       static_cast<scada::NamespaceIndex>(namespace_index)};
}
