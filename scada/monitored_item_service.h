#pragma once

#include "scada/aggregate_filter.h"
#include "scada/attribute_service.h"
#include "scada/data_change_filter.h"
#include "scada/event.h"
#include "scada/monitored_item.h"

#include <memory>
#include <optional>

namespace scada {

class MonitoredItem;
struct ReadValueId;

using MonitoringFilter = std::
    variant<std::monostate, DataChangeFilter, EventFilter, AggregateFilter>;

struct MonitoringParameters {
  bool is_null() const {
    return !sampling_interval.has_value() && filter.index() == 0 &&
           !queue_size.has_value();
  }

  MonitoringParameters& set_filter(MonitoringFilter filter) {
    this->filter = std::move(filter);
    return *this;
  }

  std::optional<Duration> sampling_interval;

  MonitoringFilter filter;

  std::optional<size_t> queue_size;
};

class MonitoredItemService {
 public:
  virtual ~MonitoredItemService() {}

  virtual std::shared_ptr<MonitoredItem> CreateMonitoredItem(
      const ReadValueId& value_id,
      const MonitoringParameters& params) = 0;
};

}  // namespace scada
