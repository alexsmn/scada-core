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

  MonitoredItem(const MonitoredItem&) = delete;
  MonitoredItem& operator=(const MonitoredItem&) = delete;

  void set_data_change_handler(DataChangeHandler handler) {
    data_change_handler_ = std::move(handler);
  }
  void set_event_handler(EventHandler handler) {
    event_handler_ = std::move(handler);
  }

  virtual void Subscribe() = 0;

  void ForwardData(const DataValue& data_value);
  void ForwardEvent(const Status& status, const std::any& event);

 protected:
  DataChangeHandler data_change_handler_;
  EventHandler event_handler_;
};

}  // namespace scada
