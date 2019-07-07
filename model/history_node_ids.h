#pragma once

#include "model/scada_node_ids.h"

namespace history {

constexpr scada::NamespaceIndex kNamespaceIndex = NamespaceIndexes::SCADA;

namespace numeric_id {

const scada::NumericId HistoricalDatabases = 12;

}  // namespace numeric_id

namespace id {

const scada::NodeId HistoricalDatabases{numeric_id::HistoricalDatabases, kNamespaceIndex};
const scada::NodeId HistoricalDatabaseType_Depth{171, kNamespaceIndex};
const scada::NodeId HistoricalDatabaseType_EventCleanupDuration{242, kNamespaceIndex};
const scada::NodeId HistoricalDatabaseType_PendingTaskCount{241, kNamespaceIndex};
const scada::NodeId HistoricalDatabaseType_ValueCleanupDuration{243, kNamespaceIndex};
const scada::NodeId HistoricalDatabaseType_WriteValueDuration{240, kNamespaceIndex};
const scada::NodeId HistoricalDatabaseType{20, kNamespaceIndex};
const scada::NodeId SystemDatabase{233, kNamespaceIndex};
const scada::NodeId HasHistoricalDatabase{152, kNamespaceIndex}; // Ref

}  // namespace id

}  // namespace history
