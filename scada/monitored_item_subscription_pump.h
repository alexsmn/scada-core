#pragma once

#include "base/any_executor.h"
#include "base/awaitable.h"
#include "scada/monitored_item.h"
#include "scada/monitored_item_service.h"
#include "scada/status.h"

#include <functional>
#include <memory>
#include <span>
#include <vector>

namespace scada {

// Owns a `MonitoredItemSubscription` and continuously drains notification
// batches on the supplied executor. Callers keep domain-specific item-id
// mapping and notification interpretation outside this helper.
class MonitoredItemSubscriptionPump {
 public:
  using NotificationBatchHandler =
      std::function<void(std::vector<MonitoredItemNotification> notifications)>;
  using ErrorHandler = std::function<void(Status status)>;

  // `notification_batch_handler` and `error_handler` are invoked on `executor`.
  MonitoredItemSubscriptionPump(
      AnyExecutor executor,
      MonitoredItemService& monitored_item_service,
      MonitoredItemSubscriptionOptions options,
      NotificationBatchHandler notification_batch_handler,
      ErrorHandler error_handler);
  ~MonitoredItemSubscriptionPump();

  MonitoredItemSubscriptionPump(const MonitoredItemSubscriptionPump&) = delete;
  MonitoredItemSubscriptionPump& operator=(
      const MonitoredItemSubscriptionPump&) = delete;

  // Creates the underlying subscription and starts the read loop. `context` is
  // handed to the backend's CreateSubscription, so its trace id links the
  // backend's subscription spans to the caller that caused them. A pump is
  // created once per backend and reused for every later item, so this is the
  // context of whichever item first routed here — accurate for a subscription
  // created per request (an E2E probe), approximate for a long-lived client
  // subscription that keeps adding items.
  [[nodiscard]] Status Start(ServiceContext context = {});

  // Adds monitored items through the owned subscription. If the pump is closed
  // or has not started, all requests complete with Bad_Disconnected.
  [[nodiscard]] Awaitable<std::vector<MonitoredItemCreateResult>> AddItems(
      std::vector<MonitoredItemCreateRequest> requests);

  // Removes monitored items through the owned subscription. If the pump is
  // closed or has not started, all ids complete with Bad_Disconnected.
  [[nodiscard]] Awaitable<std::vector<Status>> RemoveItems(
      std::span<const MonitoredItemId> item_ids);

  // Closes the underlying subscription and stops future handler delivery.
  void Close(Status status);

 private:
  struct State;

  [[nodiscard]] static Awaitable<void> ReadLoop(std::shared_ptr<State> state);

  const AnyExecutor executor_;
  MonitoredItemService& monitored_item_service_;
  const MonitoredItemSubscriptionOptions options_;
  NotificationBatchHandler notification_batch_handler_;
  ErrorHandler error_handler_;
  std::shared_ptr<State> state_;
};

}  // namespace scada
