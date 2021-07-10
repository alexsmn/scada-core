#pragma once

#include "core/monitored_item.h"

#include <gmock/gmock.h>

namespace scada {

class MockMonitoredItem : public MonitoredItem {
 public:
  MOCK_METHOD(void, Subscribe, (MonitoredItemHandler handler), (override));
};

}  // namespace scada
