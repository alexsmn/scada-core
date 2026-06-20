#pragma once

#include "scada/attribute_service.h"
#include "scada/legacy_monitored_item_adapter.h"
#include "scada/monitored_item.h"
#include "scada/node.h"

namespace scada {

class monitored_item {
 public:
  monitored_item() = default;

  template <class Handler>
  void subscribe(const node& node,
                 AttributeId attribute_id,
                 const MonitoringParameters& params,
                 Handler&& data_change_handler);

  template <class Handler>
  void subscribe_value(const node& node,
                       const MonitoringParameters& params,
                       Handler&& data_change_handler) {
    subscribe(node, AttributeId::Value, params,
              std::forward<Handler>(data_change_handler));
  }

  // Handler = void(const Status& status, const std::any& event)
  //
  // Example:
  //
  // auto monitored_item = client.node(node_id).subscribe_events(
  //     /*params*/ {},
  //     [](const scada::Status& status, const std::any& event) {
  //       if (const auto* system_event = std::any_cast<scada::Event>(&event)) {
  //         std::cout << ToString(*system_event);
  //       }
  //     });
  template <class Handler>
  void subscribe_events(const node& node,
                        const MonitoringParameters& params,
                        Handler&& event_handler);

  // Handler = void(const Status& status, const Event& system_event)
  //
  // Example:
  //
  // auto monitored_item = client.node(node_id).subscribe_events(
  //     /*params*/ {},
  //     [](const scada::Status& status, const scada::Event& system_event) {
  //         std::cout << ToString(system_event);
  //     });
  template <class Handler>
  void subscribe_system_events(const node& node,
                               const MonitoringParameters& params,
                               Handler&& system_event_handler) {
    subscribe_events(
        node, params,
        [system_event_handler = std::forward<Handler>(system_event_handler)](
            const Status& status, const std::any& event) mutable {
          if (const auto* system_event = std::any_cast<scada::Event>(&event)) {
            system_event_handler(status, *system_event);
          }
        });
  }

  bool subscribed() const { return state_ != nullptr; }

  void unsubscribe() { state_ = nullptr; }

 private:
  struct state {
    state(std::shared_ptr<LegacyMonitoredItemAdapter> adapter,
          std::shared_ptr<MonitoredItem> monitored_item);

    void handle_status(scada::StatusCode status_code);

    // Owns the subscription that backs the single monitored item below; kept
    // alive for as long as the item is subscribed.
    std::shared_ptr<LegacyMonitoredItemAdapter> adapter_;

    // Monitored item is reset only once when bad status code is received. It's
    // safe to keep it outside of mutex.
    std::shared_ptr<MonitoredItem> monitored_item_;
  };

  std::shared_ptr<state> state_;
};

// monitored_item::state

inline monitored_item::state::state(
    std::shared_ptr<LegacyMonitoredItemAdapter> adapter,
    std::shared_ptr<MonitoredItem> monitored_item)
    : adapter_{std::move(adapter)},
      monitored_item_{std::move(monitored_item)} {}

inline void monitored_item::state::handle_status(
    scada::StatusCode status_code) {
  // Release monitored item on bad status code.
  if (scada::IsBad(status_code)) {
    // It's safe to reset monitored item outside of the mutex, as it's only
    // updated once.
    monitored_item_.reset();
  }
}

// monitored_item

template <class Handler>
inline void monitored_item::subscribe(const node& node,
                                      AttributeId attribute_id,
                                      const MonitoringParameters& params,
                                      Handler&& data_change_handler) {
  assert(attribute_id != AttributeId::EventNotifier);

  if (!node.services_.monitored_item_service) {
    data_change_handler(MakeReadError(StatusCode::Bad_Disconnected));
    return;
  }

  auto adapter = std::make_shared<LegacyMonitoredItemAdapter>(
      node.services_.monitored_item_executor,
      *node.services_.monitored_item_service);
  auto item = adapter->CreateMonitoredItem(
      {.node_id = node.node_id_, .attribute_id = attribute_id}, params);

  assert(!state_);
  assert(item);

  state_ = std::make_shared<state>(std::move(adapter), item);

  item->Subscribe(
      [weak_state = std::weak_ptr{state_},
       data_change_handler = std::forward<Handler>(data_change_handler)](
          const DataValue& data_value) mutable {
        if (auto state = weak_state.lock()) {
          data_change_handler(data_value);
          // When `data_value.status_code` is bad, this call may destroy the
          // calling monitored item with the lambda and captured
          // `data_change_handler`.
          state->handle_status(data_value.status_code);
        }
      });
}

template <class Handler>
inline void monitored_item::subscribe_events(const node& node,
                                             const MonitoringParameters& params,
                                             Handler&& event_handler) {
  if (!node.services_.monitored_item_service) {
    // Must specify `std::any` to avoid ambiguity when using `BindExecutor`.
    event_handler(StatusCode::Bad_Disconnected, std::any{});
    return;
  }

  auto adapter = std::make_shared<LegacyMonitoredItemAdapter>(
      node.services_.monitored_item_executor,
      *node.services_.monitored_item_service);
  auto item = adapter->CreateMonitoredItem(
      {.node_id = node.node_id_, .attribute_id = AttributeId::EventNotifier},
      params);

  assert(!state_);
  assert(item);

  state_ = std::make_shared<state>(std::move(adapter), item);

  item->Subscribe([weak_state = std::weak_ptr{state_},
                   event_handler = std::forward<Handler>(event_handler)](
                      const Status& status, const std::any& event) mutable {
    if (auto state = weak_state.lock()) {
      state->handle_status(status.code());
      event_handler(status, event);
    }
  });
}

}  // namespace scada