#pragma once

#include "base/any_executor.h"
#include "base/awaitable.h"
#include "scada/data_value.h"
#include "scada/monitored_item_service.h"
#include "scada/monitoring_parameters.h"
#include "scada/read_value_id.h"

#include <vector>

namespace scada {

[[nodiscard]] Awaitable<DataValue> ReadInitialValueAsync(
    AnyExecutor executor,
    MonitoredItemService& monitored_item_service,
    const ReadValueId& read_value_id,
    const MonitoringParameters& params);

[[nodiscard]] Awaitable<std::vector<DataValue>> ReadInitialValuesAsync(
    AnyExecutor executor,
    MonitoredItemService& monitored_item_service,
    std::vector<ReadValueId> read_value_ids,
    const MonitoringParameters& params);

}  // namespace scada
