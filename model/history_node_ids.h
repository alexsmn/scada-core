#pragma once

#include "core/standard_node_ids.h"
#include "model/namespaces.h"

namespace history {

constexpr scada::NamespaceIndex kNamespaceIndex = NamespaceIndexes::HISTORY;

namespace numeric_id {

constexpr scada::NumericId HasHistoricalDatabase = 15001; // ReferenceType
constexpr scada::NumericId HistoricalDatabaseType = 20; // ObjectType
constexpr scada::NumericId HistoricalDatabaseType_Depth = 36; // Variable
constexpr scada::NumericId HistoricalDatabaseType_EventCleanupDuration = 51; // Variable
constexpr scada::NumericId HistoricalDatabaseType_PendingTaskCount = 50; // Variable
constexpr scada::NumericId HistoricalDatabaseType_ValueCleanupDuration = 52; // Variable
constexpr scada::NumericId HistoricalDatabaseType_WriteValueDuration = 15002; // Variable
constexpr scada::NumericId HistoricalDatabaseType_WrittenEventCount = 15022; // Variable
constexpr scada::NumericId HistoricalDatabaseType_WrittenValueCount = 15021; // Variable
constexpr scada::NumericId HistoricalDatabases = 12; // Object
constexpr scada::NumericId SystemDatabase = 13; // Object
constexpr scada::NumericId SystemDatabase_Depth = 37; // Variable
constexpr scada::NumericId SystemDatabase_EventCleanupDuration = 55; // Variable
constexpr scada::NumericId SystemDatabase_PendingTaskCount = 54; // Variable
constexpr scada::NumericId SystemDatabase_ValueCleanupDuration = 56; // Variable
constexpr scada::NumericId SystemDatabase_WriteValueDuration = 15003; // Variable
constexpr scada::NumericId SystemDatabase_WrittenEventCount = 15024; // Variable
constexpr scada::NumericId SystemDatabase_WrittenValueCount = 15023; // Variable

} //  namespace numeric_id

namespace id {

const scada::NodeId HasHistoricalDatabase{numeric_id::HasHistoricalDatabase, kNamespaceIndex}; // ReferenceType
const scada::NodeId HistoricalDatabaseType{numeric_id::HistoricalDatabaseType, kNamespaceIndex}; // ObjectType
const scada::NodeId HistoricalDatabaseType_Depth{numeric_id::HistoricalDatabaseType_Depth, kNamespaceIndex}; // Variable
const scada::NodeId HistoricalDatabaseType_EventCleanupDuration{numeric_id::HistoricalDatabaseType_EventCleanupDuration, kNamespaceIndex}; // Variable
const scada::NodeId HistoricalDatabaseType_PendingTaskCount{numeric_id::HistoricalDatabaseType_PendingTaskCount, kNamespaceIndex}; // Variable
const scada::NodeId HistoricalDatabaseType_ValueCleanupDuration{numeric_id::HistoricalDatabaseType_ValueCleanupDuration, kNamespaceIndex}; // Variable
const scada::NodeId HistoricalDatabaseType_WriteValueDuration{numeric_id::HistoricalDatabaseType_WriteValueDuration, kNamespaceIndex}; // Variable
const scada::NodeId HistoricalDatabaseType_WrittenEventCount{numeric_id::HistoricalDatabaseType_WrittenEventCount, kNamespaceIndex}; // Variable
const scada::NodeId HistoricalDatabaseType_WrittenValueCount{numeric_id::HistoricalDatabaseType_WrittenValueCount, kNamespaceIndex}; // Variable
const scada::NodeId HistoricalDatabases{numeric_id::HistoricalDatabases, kNamespaceIndex}; // Object
const scada::NodeId SystemDatabase{numeric_id::SystemDatabase, kNamespaceIndex}; // Object
const scada::NodeId SystemDatabase_Depth{numeric_id::SystemDatabase_Depth, kNamespaceIndex}; // Variable
const scada::NodeId SystemDatabase_EventCleanupDuration{numeric_id::SystemDatabase_EventCleanupDuration, kNamespaceIndex}; // Variable
const scada::NodeId SystemDatabase_PendingTaskCount{numeric_id::SystemDatabase_PendingTaskCount, kNamespaceIndex}; // Variable
const scada::NodeId SystemDatabase_ValueCleanupDuration{numeric_id::SystemDatabase_ValueCleanupDuration, kNamespaceIndex}; // Variable
const scada::NodeId SystemDatabase_WriteValueDuration{numeric_id::SystemDatabase_WriteValueDuration, kNamespaceIndex}; // Variable
const scada::NodeId SystemDatabase_WrittenEventCount{numeric_id::SystemDatabase_WrittenEventCount, kNamespaceIndex}; // Variable
const scada::NodeId SystemDatabase_WrittenValueCount{numeric_id::SystemDatabase_WrittenValueCount, kNamespaceIndex}; // Variable

} //  namespace id

} //  namespace history

