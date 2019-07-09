#pragma once

#include "core/configuration_types.h"

#include <any>
#include <functional>

namespace scada {

class DataValue;
class Status;

typedef std::function<void(const DataValue& data_value)> DataChangeHandler;
typedef std::function<void(const Status& status, const std::any& event)>
    EventHandler;

class MonitoredItem {
 public:
  MonitoredItem() {}
  virtual ~MonitoredItem() {}

  virtual void SubscribeData(DataChangeHandler handler) = 0;
  virtual void SubscribeEvents(EventHandler handler) = 0;
};

}  // namespace scada
