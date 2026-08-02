#include "scada/monitored_item_service_awaitable.h"

#include "base/check.h"
#include "scada/attribute_service.h"
#include "scada/status.h"

#include <boost/asio/steady_timer.hpp>

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <limits>
#include <memory>

namespace scada {
namespace {

void CompleteWithStatus(std::vector<DataValue>& results,
                        std::vector<bool>& completed,
                        std::size_t& remaining,
                        std::size_t index,
                        StatusCode status_code) {
  if (index >= results.size() || completed[index]) {
    return;
  }

  results[index] = MakeReadError(status_code);
  completed[index] = true;
  --remaining;
}

void CompleteWithValue(std::vector<DataValue>& results,
                       std::vector<bool>& completed,
                       std::size_t& remaining,
                       std::size_t index,
                       DataValue value) {
  if (index >= results.size() || completed[index]) {
    return;
  }

  results[index] = std::move(value);
  completed[index] = true;
  --remaining;
}

}  // namespace

Awaitable<DataValue> ReadInitialValueAsync(
    AnyExecutor executor,
    MonitoredItemService& monitored_item_service,
    ReadValueId read_value_id,
    MonitoringParameters params,
    std::chrono::steady_clock::duration timeout) {
  auto values = co_await ReadInitialValuesAsync(
      std::move(executor), monitored_item_service,
      std::vector<ReadValueId>{std::move(read_value_id)}, std::move(params),
      timeout);
  base::Check(values.size() == 1);
  co_return std::move(values.front());
}

Awaitable<std::vector<DataValue>> ReadInitialValuesAsync(
    AnyExecutor executor,
    MonitoredItemService& monitored_item_service,
    std::vector<ReadValueId> read_value_ids,
    MonitoringParameters params,
    std::chrono::steady_clock::duration timeout) {
  std::vector<DataValue> results(read_value_ids.size());
  std::vector<bool> completed(read_value_ids.size());
  std::size_t remaining = read_value_ids.size();
  if (read_value_ids.empty()) {
    co_return results;
  }

  auto subscription_result = monitored_item_service.CreateSubscription(
      ServiceContext{}, {.max_batch_size = read_value_ids.size()});
  if (!subscription_result.ok()) {
    std::ranges::fill(results,
                      MakeReadError(subscription_result.status().code()));
    co_return results;
  }

  // Shared so the timeout handler below can outlive this coroutine safely.
  const std::shared_ptr<MonitoredItemSubscription> subscription =
      std::move(*subscription_result);
  std::vector<MonitoredItemCreateRequest> requests;
  requests.reserve(read_value_ids.size());
  for (std::size_t i = 0; i < read_value_ids.size(); ++i) {
    const auto client_handle = i > std::numeric_limits<std::uint32_t>::max()
                                   ? std::numeric_limits<std::uint32_t>::max()
                                   : static_cast<std::uint32_t>(i);
    requests.emplace_back(MonitoredItemCreateRequest{
        .item_to_monitor = std::move(read_value_ids[i]),
        .parameters = params,
        .client_handle = client_handle});
  }

  auto add_results = co_await subscription->AddItems(std::move(requests));
  std::vector<bool> add_result_seen(results.size());
  for (const auto& add_result : add_results) {
    const auto index = static_cast<std::size_t>(add_result.client_handle);
    if (index >= results.size()) {
      continue;
    }

    add_result_seen[index] = true;
    if (!add_result.status) {
      CompleteWithStatus(results, completed, remaining, index,
                         add_result.status.code());
    }
  }

  for (std::size_t i = 0; i < add_result_seen.size(); ++i) {
    if (!add_result_seen[i]) {
      CompleteWithStatus(results, completed, remaining, i, StatusCode::Bad);
    }
  }

  // Bound the wait. Closing the subscription makes the pending (and any
  // subsequent) ReadNext return the close status, which the loop below turns
  // into Bad_Timeout for every item that has not reported yet. Without this an
  // item whose source never writes a value leaves the read suspended forever.
  std::shared_ptr<boost::asio::steady_timer> timeout_timer;
  if (executor) {
    timeout_timer = std::make_shared<boost::asio::steady_timer>(executor);
    timeout_timer->expires_after(timeout);
    timeout_timer->async_wait(
        [timeout_timer, subscription](boost::system::error_code error) {
          if (error)  // cancelled after the read completed
            return;
          subscription->Close(StatusCode::Bad_Timeout);
        });
  }

  while (remaining != 0) {
    auto notifications = co_await subscription->ReadNext(remaining);
    if (!notifications.ok()) {
      for (std::size_t i = 0; i < results.size(); ++i) {
        CompleteWithStatus(results, completed, remaining, i,
                           notifications.status().code());
      }
      break;
    }

    bool overflowed = false;
    for (const auto& notification : *notifications) {
      if (const auto* data_change =
              std::get_if<DataChangeNotification>(&notification)) {
        CompleteWithValue(results, completed, remaining,
                          data_change->client_handle, data_change->value);
      } else if (const auto* status =
                     std::get_if<ItemStatusNotification>(&notification)) {
        CompleteWithStatus(results, completed, remaining, status->client_handle,
                           status->status.code());
      } else if (const auto* overflow =
                     std::get_if<OverflowNotification>(&notification)) {
        for (std::size_t i = 0; i < results.size(); ++i) {
          CompleteWithStatus(results, completed, remaining, i,
                             overflow->status.code());
        }
        overflowed = true;
        break;
      }
    }

    if (overflowed)
      break;
  }

  if (timeout_timer)
    timeout_timer->cancel();

  co_return results;
}

}  // namespace scada
