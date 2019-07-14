#pragma once

#include "core/monitored_item.h"

#include <gmock/gmock.h>

namespace scada {

class MockMonitoredItem : public MonitoredItem {
 public:
  MOCK_METHOD1(SubscribeData, void(DataChangeHandler handler));
  MOCK_METHOD1(SubscribeEvents, void(EventHandler handler));
};

}  // namespace scada