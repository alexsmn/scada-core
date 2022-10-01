#pragma once

#include "remote/subscription.h"

class MonitoredItemProxy;

class MonitoredItemRouter {
 public:
  virtual ~MonitoredItemRouter() {}

  virtual void AddMonitoredItemDataObserver(MonitoredItemId monitored_item_id,
                                            MonitoredItemProxy& item) = 0;
  virtual void RemoveMonitoredItemDataObserver(
      MonitoredItemId monitored_item_id) = 0;
};
