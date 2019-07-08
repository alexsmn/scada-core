#include "core/monitored_item.h"

#include "core/status.h"

namespace scada {

void MonitoredItem::ForwardData(const DataValue& data_value) {
  if (data_change_handler_)
    data_change_handler_(data_value);
}

void MonitoredItem::ForwardEvent(const Status& status, const std::any& event) {
  if (event_handler_)
    event_handler_(status, event);
}

}  // namespace scada