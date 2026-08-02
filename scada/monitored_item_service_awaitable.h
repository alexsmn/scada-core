#pragma once

#include "base/any_executor.h"
#include "base/awaitable.h"
#include "scada/data_value.h"
#include "scada/monitored_item_service.h"
#include "scada/monitoring_parameters.h"
#include "scada/read_value_id.h"

#include <chrono>
#include <vector>

namespace scada {

// Upper bound on how long an initial-value read waits for its monitored items
// to report. The wait itself is legitimate — a device value often arrives just
// after the item is created — but an item whose source never writes a value
// would otherwise never report at all and suspend the read forever. Items still
// outstanding when this elapses complete with `Bad_Timeout`.
inline constexpr std::chrono::seconds kDefaultInitialValueTimeout{5};

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
    MonitoringParameters params,
    std::chrono::steady_clock::duration timeout = kDefaultInitialValueTimeout);

// Batch variant of `ReadInitialValueAsync`: reads the current values of
// `read_value_ids` and returns them in input order.
[[nodiscard]] Awaitable<std::vector<DataValue>> ReadInitialValuesAsync(
    AnyExecutor executor,
    MonitoredItemService& monitored_item_service,
    std::vector<ReadValueId> read_value_ids,
    MonitoringParameters params,
    std::chrono::steady_clock::duration timeout = kDefaultInitialValueTimeout);

}  // namespace scada
