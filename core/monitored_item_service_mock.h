#pragma once

#include "core/monitored_item_service.h"

namespace scada {

class MockMonitoredItemService : public MonitoredItemService {
 public:
  MOCK_METHOD2(
      CreateMonitoredItem,
      std::shared_ptr<MonitoredItem>(const ReadValueId& value_id,
                                     const MonitoringParameters& params));
};

}  // namespace scada
