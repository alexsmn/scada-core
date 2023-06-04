#pragma once

#include "core/basic_types.h"

#include <string_view>

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
constexpr scada::NamespaceIndex IEC60870_TRANSMISSION_ITEM = 16;
constexpr scada::NamespaceIndex IEC61850_DEVICE = 17;
constexpr scada::NamespaceIndex IEC61850_RCB = 18;
constexpr scada::NamespaceIndex IEC61850_TRANSMISSION_ITEM = 27;
// constexpr scada::NamespaceIndex IEC61850_SERVER = 22;
// constexpr scada::NamespaceIndex OPC_SERVER = 25;
constexpr scada::NamespaceIndex MODBUS_TRANSMISSION_ITEM = 26;

constexpr scada::NamespaceIndex SCADA = 7;
// constexpr scada::NamespaceIndex HISTORY = 8;
// constexpr scada::NamespaceIndex DEVICES = 13;
// constexpr scada::NamespaceIndex FILESYSTEM = 19;
constexpr scada::NamespaceIndex FILESYSTEM_FILE = 24;
// constexpr scada::NamespaceIndex DATA_ITEMS = 20;
// constexpr scada::NamespaceIndex SECURITY = 21;
constexpr scada::NamespaceIndex ALIAS = 23;
// constexpr scada::NamespaceIndex METRIC = 24;
constexpr scada::NamespaceIndex VIDICON = 28;

constexpr scada::NamespaceIndex END = 29;

}  // namespace NamespaceIndexes

std::string_view GetNamespaceName(scada::NamespaceIndex namespace_index);
int FindNamespaceIndexByName(std::string_view name);
