#pragma once

#include <memory>

namespace scada {

class MonitoredItem;
struct MonitoringParameters;
struct ReadValueId;

class MonitoredItemService {
 public:
  virtual ~MonitoredItemService() {}

  virtual std::shared_ptr<MonitoredItem> CreateMonitoredItem(
      const ReadValueId& value_id,
      const MonitoringParameters& params) = 0;
};

}  // namespace scada
