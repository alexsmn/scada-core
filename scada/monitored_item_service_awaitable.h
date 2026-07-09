#pragma once

#include "base/any_executor.h"
#include "base/awaitable.h"
#include "scada/data_value.h"
#include "scada/monitored_item_service.h"
#include "scada/monitoring_parameters.h"
#include "scada/read_value_id.h"

#include <vector>

namespace scada {

// Reads the current value of `read_value_id` by creating a temporary
// monitored item and awaiting its first notification.
//
// These are lazy coroutines: value-type parameters are taken by value (not
// const&) so the coroutine frame owns copies. A const& parameter bound to a
// caller temporary would dangle, because the body first runs after the
// caller's full expression has ended. `monitored_item_service` must outlive
// the returned awaitable.
[[nodiscard]] Awaitable<DataValue> ReadInitialValueAsync(
    AnyExecutor executor,
    MonitoredItemService& monitored_item_service,
    ReadValueId read_value_id,
    MonitoringParameters params);

// Batch variant of `ReadInitialValueAsync`: reads the current values of
// `read_value_ids` and returns them in input order.
[[nodiscard]] Awaitable<std::vector<DataValue>> ReadInitialValuesAsync(
    AnyExecutor executor,
    MonitoredItemService& monitored_item_service,
    std::vector<ReadValueId> read_value_ids,
    MonitoringParameters params);

}  // namespace scada
