#pragma once

#include "base/any_executor.h"
#include "scada/monitored_item.h"
#include "scada/monitored_item_service.h"
#include "scada/service_context.h"

#include <memory>

namespace scada {

// Adapts the legacy single-item `MonitoredItem` API to a shared
// `MonitoredItemSubscriptionPump`. Each created legacy item is added to the
// same subscription and receives only its matching data-change notifications.
class LegacyMonitoredItemAdapter {
 public:
  // `context` is the caller the shared subscription is created for: the
  // service judges by it what the items may receive (a server withholds events
  // the caller lacks ReceiveEvents for, OPC UA Part 3 §8.55 bit 11). The
  // default is anonymous.
  LegacyMonitoredItemAdapter(AnyExecutor executor,
                             MonitoredItemService& service,
                             MonitoredItemSubscriptionOptions options = {},
                             ServiceContext context = {});
  ~LegacyMonitoredItemAdapter();

  LegacyMonitoredItemAdapter(const LegacyMonitoredItemAdapter&) = delete;
  LegacyMonitoredItemAdapter& operator=(const LegacyMonitoredItemAdapter&) =
      delete;

  std::shared_ptr<MonitoredItem> CreateMonitoredItem(
      ReadValueId value_id,
      MonitoringParameters params);

  void Close(Status status);

 private:
  struct ItemState;
  struct State;

  class SubscriptionBackedMonitoredItem;

  static void AddItem(std::shared_ptr<State> state,
                      std::shared_ptr<ItemState> item_state);
  static void OnNotifications(
      std::weak_ptr<State> weak_state,
      std::vector<MonitoredItemNotification> notifications);
  static void RemoveItem(std::shared_ptr<State> state,
                         std::uint32_t client_handle,
                         MonitoredItemId item_id);
  static void EraseItemMapping(std::shared_ptr<State> state,
                               std::uint32_t client_handle);
  static void CloseItem(std::shared_ptr<ItemState> item_state);
  static void CloseAll(std::weak_ptr<State> weak_state, Status status);
  static void CloseAll(std::shared_ptr<State> state, Status status);

  const std::shared_ptr<State> state_;
};

}  // namespace scada
