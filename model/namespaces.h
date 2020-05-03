#pragma once

#include "base/strings/string_piece.h"
#include "core/basic_types.h"

namespace NamespaceIndexes {

constexpr scada::NamespaceIndex NS0 = 0;
constexpr scada::NamespaceIndex TS = 1;
constexpr scada::NamespaceIndex TIT = 2;
constexpr scada::NamespaceIndex MODBUS_DEVICES = 3;
constexpr scada::NamespaceIndex GROUP = 4;
constexpr scada::NamespaceIndex USER = 5;
constexpr scada::NamespaceIndex HISTORICAL_DB = 6;
constexpr scada::NamespaceIndex SIM_ITEM = 9;
constexpr scada::NamespaceIndex IEC60870_LINK = 10;
constexpr scada::NamespaceIndex IEC60870_DEVICE = 11;
constexpr scada::NamespaceIndex MODBUS_PORTS = 12;
constexpr scada::NamespaceIndex TS_FORMAT = 14;
constexpr scada::NamespaceIndex SERVER_PARAMS = 15;
constexpr scada::NamespaceIndex TRANSMISSION_ITEM = 16;
constexpr scada::NamespaceIndex IEC61850_DEVICE = 17;
constexpr scada::NamespaceIndex IEC61850_RCB = 18;

constexpr scada::NamespaceIndex SCADA = 7;

constexpr scada::NamespaceIndex END = 19;

}  // namespace NamespaceIndexes

base::StringPiece GetNamespaceName(scada::NamespaceIndex namespace_index);
int FindNamespaceIndexByName(base::StringPiece name);