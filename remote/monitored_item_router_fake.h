#pragma once

#include "base/check.h"
#include "remote/monitored_item_router.h"

#include <map>
#include <vector>

class MonitoredItemProxy;

// Records what a monitored-item router ends up holding, so a test can assert
// on the registration state itself rather than on the calls that produced it.
//
// Mirrors SubscriptionProxy, the only production implementation, including its
// invariants: registering an id twice, or removing one that was never
// registered, is a Check failure there and is one here too. A fake that
// tolerated either would let a caller drift into behaviour the real router
// panics on, which is the failure a shared fake exists to prevent.
//
// The real class cannot stand in for this. SubscriptionProxy derives from
// MonitoredItemRouter *privately* and befriends MonitoredItemProxy, so it
// offers no upcast a test could use -- wiring the production implementation in
// would mean changing that inheritance, which is a production change this test
// does not justify on its own.
class MonitoredItemRouterFake : public MonitoredItemRouter {
 public:
  void AddMonitoredItemDataObserver(MonitoredItemId monitored_item_id,
                                    MonitoredItemProxy& item) override {
    scada::base::Check(!observers_.contains(monitored_item_id),
                       "monitored item registered twice");
    observers_.emplace(monitored_item_id, &item);
  }

  void RemoveMonitoredItemDataObserver(
      MonitoredItemId monitored_item_id) override {
    scada::base::Check(observers_.contains(monitored_item_id),
                       "monitored item removed without being registered");
    observers_.erase(monitored_item_id);
  }

  // The ids currently registered, ascending.
  std::vector<MonitoredItemId> registered_ids() const {
    std::vector<MonitoredItemId> ids;
    ids.reserve(observers_.size());
    for (const auto& [id, item] : observers_) {
      ids.push_back(id);
    }
    return ids;
  }

  // The proxy registered under `monitored_item_id`, or nullptr if none is.
  const MonitoredItemProxy* observer(MonitoredItemId monitored_item_id) const {
    auto i = observers_.find(monitored_item_id);
    return i == observers_.end() ? nullptr : i->second;
  }

 private:
  std::map<MonitoredItemId, MonitoredItemProxy*> observers_;
};
