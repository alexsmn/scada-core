#pragma once

#include "base/promise.h"
#include "core/monitored_item.h"
#include "core/monitored_item_service.h"

namespace scada {

inline promise<scada::DataValue> ReadInitialValue(
    scada::MonitoredItemService& monitored_item_service,
    const scada::ReadValueId& read_value_id,
    const scada::MonitoringParameters& params) {
  auto monitored_item =
      monitored_item_service.CreateMonitoredItem(read_value_id, params);
  if (!monitored_item) {
    return make_resolved_promise(
        scada::MakeReadError(scada::StatusCode::Bad_WrongNodeId));
  }

  promise<scada::DataValue> promise;
  // WARNING: Self-reference |monitored_item|.
  monitored_item->Subscribe([promise, monitored_item, completed = false](
                                const scada::DataValue& data_value) mutable {
    if (!completed) {
      completed = true;
      monitored_item.reset();
      promise.resolve(data_value);
    }
  });
  return promise;
}

}  // namespace scada
