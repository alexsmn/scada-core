#pragma once

#include "base/promise.h"
#include "scada/data_value.h"
#include "scada/monitored_item.h"
#include "scada/status.h"

namespace scada {

// TODO: Must be thread-safe, as it's used in history tests. Historical
// databases run in separate threads.
class TestMonitoredItem : public MonitoredItem {
 public:
  virtual void Subscribe(MonitoredItemHandler handler) override {
    assert(!data_change_handler_);
    assert(!event_handler_);

    if (auto* data_change_handler = std::get_if<DataChangeHandler>(&handler)) {
      data_change_handler_ = std::move(*data_change_handler);
      if (!data_value_.is_null()) {
        data_change_handler_(data_value_);
      }
    } else if (auto* event_handler = std::get_if<EventHandler>(&handler)) {
      event_handler_ = std::move(*event_handler);
    } else {
      assert(false);
    }

    subscribed_promise_.resolve();
  }

  bool subscribed() const { return event_handler_ != nullptr; }

  void WaitForSubscription() { subscribed_promise_.get(); }

  void NotifyDataChange(const scada::DataValue& data_value) {
    data_value_ = data_value;

    if (data_change_handler_) {
      data_change_handler_(data_value);
    }
  }

  void NotifyEvent(const std::any& event) {
    if (!event_handler_)
      return;

    event_handler_(StatusCode::Good, event);
  }

 private:
  DataChangeHandler data_change_handler_;
  EventHandler event_handler_;

  promise<void> subscribed_promise_;

  // TODO: Thread-safe.
  scada::DataValue data_value_;
};

}  // namespace scada
