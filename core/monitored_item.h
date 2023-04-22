#pragma once

#include <any>
#include <functional>
#include <variant>

namespace scada {

class DataValue;
class Status;

// When first subscribed, an initial value is sent. Initial value may be null.
using DataChangeHandler = std::function<void(const DataValue& data_value)>;

// When first subscribed, an initial `status` update is sent. Initial status
// update may have or may not have an `event`. WARNING: `event` may be not set
// for initial status update.
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
