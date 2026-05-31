#pragma once

#include "scada/monitored_item.h"
#include "scada/service_context.h"
#include "scada/status_or.h"

#include <memory>

namespace scada {

class MonitoredItemService {
 public:
  virtual ~MonitoredItemService() {}

  virtual std::shared_ptr<MonitoredItem> CreateMonitoredItem(
      const ReadValueId& value_id,
      const MonitoringParameters& params) = 0;

  virtual StatusOr<std::unique_ptr<MonitoredItemSubscription>>
  CreateSubscription(ServiceContext context,
                     MonitoredItemSubscriptionOptions options);
};

}  // namespace scada
