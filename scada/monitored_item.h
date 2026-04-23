#pragma once

#include <any>
#include <functional>
#include <variant>

namespace scada {

class DataValue;
class Status;

// When first subscribed, a cached value is sent immediately if one is already
// available. If no cached value exists yet, the first callback is expected to
// arrive once the initial value or status becomes available from the source.
using DataChangeHandler = std::function<void(const DataValue& data_value)>;

// When first subscribed, a cached status update may be sent immediately if one
// is already available. Otherwise the first callback is expected once the
// initial status (and optional event payload) becomes available from the
// source. WARNING: `event` may be not set for the initial status update.
using EventHandler =
    std::function<void(const Status& status, const std::any& event)>;

using MonitoredItemHandler = std::variant<DataChangeHandler, EventHandler>;

class MonitoredItem {
 public:
  MonitoredItem() {}
  virtual ~MonitoredItem() = default;

  virtual void Subscribe(MonitoredItemHandler handler) = 0;
};

}  // namespace scada
