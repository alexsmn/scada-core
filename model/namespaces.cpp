#include "model/namespaces.h"

#include "base/string_piece_util.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_util.h"

#include <cstring>

namespace {

// WARNING: These names are used as names for tables in the configuration DB and
// mustn't be modified.
constexpr std::string_view kNamespaceNames[] = {
    "NS0",          "TS",           "TIT",           "MODBUS_DEVICES",
    "GROUP",        "USER",         "HISTORICAL_DB", "SCADA",
    "EXPR",         "SIM_ITEM",     "IEC_LINK",      "IEC_DEV",
    "MODBUS_PORTS", "FILE",         "TS_PARAMS",     "SERVER_PARAMS",
    "IEC_TRANSMIT", "IEC61850_DEV", "IEC61850_RCB",
};

static_assert(std::size(kNamespaceNames) == NamespaceIndexes::END);

}  // namespace

std::string_view GetNamespaceName(scada::NamespaceIndex namespace_index) {
  if (namespace_index >= 0 && namespace_index < NamespaceIndexes::END)
    return kNamespaceNames[namespace_index];
  else
    return {};
}

int FindNamespaceIndexByName(std::string_view name) {
  if (name.empty())
    return -1;

  int namespace_index = -1;
  if (name[0] == 'T' &&
      base::StringToInt(ToStringPiece(name.substr(1)), &namespace_index))
    return namespace_index;

  if (base::StringToInt(ToStringPiece(name), &namespace_index))
    return namespace_index;

  for (int i = 0; i != NamespaceIndexes::END; ++i) {
    if (base::EqualsCaseInsensitiveASCII(ToStringPiece(GetNamespaceName(i)),
                                         ToStringPiece(name)))
      return i;
  }

  return -1;
}
