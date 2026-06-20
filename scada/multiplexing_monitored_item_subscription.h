#pragma once

#include "base/any_executor.h"
#include "scada/monitored_item.h"
#include "scada/read_value_id.h"
#include "scada/status_or.h"

#include <functional>
#include <memory>
#include <vector>

namespace scada {

class MonitoredItemService;

// One destination for a monitored item: the backend service to monitor through
// and the (possibly translated) value id to monitor on that backend.
struct MonitoredItemRoute {
  MonitoredItemService* service = nullptr;
  ReadValueId value_id;
};

// Resolves a requested item to zero or more backend destinations. Returning an
// empty vector means no backend owns the item (reported as a per-item
// Bad_WrongNodeId / Bad_WrongAttributeId). Returning more than one destination
// fans the item out so all backends' notification streams are merged under the
// single item (used for EventNotifier aggregation).
using MonitoredItemRouter = std::function<std::vector<MonitoredItemRoute>(
    const ReadValueId& value_id,
    const MonitoringParameters& params)>;

// Builds a `MonitoredItemSubscription` that, for each added item, calls `router`
// to find the backend service(s), drives each backend through its own
// `MonitoredItemSubscriptionPump` on `executor`, and merges all backends'
// notifications into one stream. This keeps services that delegate or route to
// other services on the proper subscription API instead of the legacy
// single-item adapter.
StatusOr<std::unique_ptr<MonitoredItemSubscription>>
MakeMultiplexingMonitoredItemSubscription(
    AnyExecutor executor,
    MonitoredItemRouter router,
    MonitoredItemSubscriptionOptions options);

}  // namespace scada
