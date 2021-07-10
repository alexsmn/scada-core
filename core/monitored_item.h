#pragma once

#include <any>
#include <functional>
#include <variant>

namespace scada {

class DataValue;
class Status;

using DataChangeHandler = std::function<void(const DataValue& data_value)>;
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
