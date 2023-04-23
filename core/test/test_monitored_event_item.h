#pragma once

#include "base/promise.h"
#include "core/monitored_item.h"

namespace scada {

// TODO: Must be thread-safe, as it's used in history tests. Historical
// databases run in separate threads.
class TestMonitoredEventItem : public MonitoredItem {
 public:
  virtual void Subscribe(MonitoredItemHandler handler) override {
    assert(!event_handler_);

    event_handler_ = std::move(std::get<EventHandler>(handler));
    assert(event_handler_);

    subscribed_promise_.resolve();
  }

  bool subscribed() const { return event_handler_ != nullptr; }

  void WaitForSubscription() { subscribed_promise_.get(); }

  void NotifyEvent(const std::any& event) {
    if (!event_handler_)
      return;

    event_handler_(StatusCode::Good, event);
  }

 private:
  EventHandler event_handler_;

  promise<void> subscribed_promise_;
};

}  // namespace scada
