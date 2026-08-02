#pragma once

#include "remote/monitored_item_router.h"

#include <gmock/gmock.h>

class MonitoredItemRouterMock : public MonitoredItemRouter {
 public:
  MOCK_METHOD2(AddMonitoredItemDataObserver,
               void(MonitoredItemId monitored_item_id,
                    MonitoredItemProxy& item));
  MOCK_METHOD1(RemoveMonitoredItemDataObserver,
               void(MonitoredItemId monitored_item_id));
};
