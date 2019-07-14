#pragma once

#include "core/aggregate_filter.h"
#include "core/event.h"
#include "core/monitored_item.h"

#include <memory>

namespace scada {

class MonitoredItem;
struct ReadValueId;

using MonitoringFilter =
    std::variant<std::monostate, AggregateFilter, EventFilter>;

struct MonitoringParameters {
  bool is_null() const { return filter.index() == 0; }

  MonitoringFilter filter;
};

class MonitoredItemService {
 public:
  virtual ~MonitoredItemService() {}

  virtual std::shared_ptr<MonitoredItem> CreateMonitoredItem(
      const ReadValueId& value_id,
      const MonitoringParameters& params) = 0;
};

}  // namespace scada
