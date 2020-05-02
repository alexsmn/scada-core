#pragma once

#include "model/namespaces.h"

namespace history {

namespace numeric_id {

const scada::NumericId HistoricalDatabases = 12;

}  // namespace numeric_id

namespace id {

const scada::NodeId PropertyCategories_History{317, NamespaceIndexes::SCADA};
const scada::NodeId HistoricalDatabases{numeric_id::HistoricalDatabases, NamespaceIndexes::SCADA};
const scada::NodeId HistoricalDatabaseType_Depth{171, NamespaceIndexes::SCADA};
const scada::NodeId HistoricalDatabaseType_EventCleanupDuration{242, NamespaceIndexes::SCADA};
const scada::NodeId HistoricalDatabaseType_PendingTaskCount{241, NamespaceIndexes::SCADA};
const scada::NodeId HistoricalDatabaseType_ValueCleanupDuration{243, NamespaceIndexes::SCADA};
const scada::NodeId HistoricalDatabaseType_WriteValueDuration{240, NamespaceIndexes::SCADA};
const scada::NodeId HistoricalDatabaseType{20, NamespaceIndexes::SCADA};
const scada::NodeId HasHistoricalDatabase{152, NamespaceIndexes::SCADA}; // Ref
const scada::NodeId SystemDatabase{233, NamespaceIndexes::SCADA};

}

}  // namespace history
