#include "model/namespaces.h"

#include <boost/algorithm/string/predicate.hpp>

#include <cstring>

#if defined(SCADA_USE_CORE_MODULE)
// Modules-pilot consumer (SCADA_CXX_MODULES=ON): base/scada names come from
// the scada.core facade (which export-imports scada.base). The import sits
// after the textual includes because the reverse order trips an AppleClang
// 21 declaration-merging bug in libc++.
import scada.core;
#else
#include "base/format.h"
#endif

namespace {

// WARNING: These names are used as names for tables in the configuration DB and
// mustn't be modified.
constexpr std::string_view kNamespaceNames[] = {
    "NS0",
    "TS",
    "TIT",
    "MODBUS_DEVICES",
    "GROUP",
    "USER",
    "HISTORICAL_DB",
    "SCADA",
    "HISTORY",
    "SIM_ITEM",
    "IEC_LINK",
    "IEC_DEV",
    "MODBUS_PORTS",
    "DEVICES",
    "TS_PARAMS",
    "SERVER_PARAMS",
    "IEC_TRANSMIT",
    "IEC61850_DEV",
    "IEC61850_RCB",
    "FILESYSTEM",
    "DATA_ITEMS",
    "SECURITY",
    "IEC61850_SERVER",
    "ALIAS",
    "FILESYSTEM_FILE",
    "OPC",
    "MODBUS_TRANSMIT",
    "IEC61850_TRANSMIT",
    "VIDICON",
    "VIDICON_FILE",
    "ROLE",
    "ROLE_IDENTITY",
    "CONFIGURATION",
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
  if (name[0] == 'T' && Parse(name.substr(1), namespace_index)) {
    return namespace_index;
  }

  if (Parse(name, namespace_index))
    return namespace_index;

  for (scada::NamespaceIndex i = 0; i != NamespaceIndexes::END; ++i) {
    if (boost::iequals(GetNamespaceName(i), name)) {
      return i;
    }
  }

  return -1;
}
