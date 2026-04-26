#pragma once

#include "base/any_executor.h"
#include "base/awaitable.h"
#include "scada/attribute_service.h"
#include "scada/callback_awaitable.h"
#include "scada/data_value.h"
#include "scada/monitored_item.h"
#include "scada/monitored_item_service.h"
#include "scada/monitoring_parameters.h"
#include "scada/read_value_id.h"
#include "scada/status.h"

#include <memory>
#include <vector>

namespace scada {

[[nodiscard]] inline Awaitable<DataValue> ReadInitialValueAsync(
    AnyExecutor executor,
    MonitoredItemService& monitored_item_service,
    const ReadValueId& read_value_id,
    const MonitoringParameters& params) {
  auto value_id = read_value_id;
  auto monitoring_params = params;
  co_return co_await AwaitCallbackValue<DataValue>(
      std::move(executor), [&, value_id = std::move(value_id),
                            monitoring_params = std::move(monitoring_params)](
                               auto callback) mutable {
        auto monitored_item = monitored_item_service.CreateMonitoredItem(
            value_id, monitoring_params);
        if (!monitored_item) {
          callback(MakeReadError(StatusCode::Bad_WrongNodeId));
          return;
        }

        monitored_item->Subscribe(
            [callback = std::move(callback),
             monitored_item = std::move(monitored_item),
             completed = false](const DataValue& data_value) mutable {
              if (completed) {
                return;
              }

              completed = true;
              auto release_item = std::move(monitored_item);
              callback(data_value);
            });
      });
}

[[nodiscard]] inline Awaitable<std::vector<DataValue>> ReadInitialValuesAsync(
    AnyExecutor executor,
    MonitoredItemService& monitored_item_service,
    std::vector<ReadValueId> read_value_ids,
    const MonitoringParameters& params) {
  auto monitoring_params = params;
  struct State {
    explicit State(size_t size)
        : results(size), completed(size), remaining(size) {}

    std::vector<DataValue> results;
    std::vector<bool> completed;
    std::vector<std::shared_ptr<MonitoredItem>> monitored_items;
    size_t remaining = 0;
  };

  co_return co_await AwaitCallbackValue<std::vector<DataValue>>(
      std::move(executor), [&, read_value_ids = std::move(read_value_ids),
                            monitoring_params = std::move(monitoring_params)](
                               auto callback) mutable {
        auto state = std::make_shared<State>(read_value_ids.size());
        if (read_value_ids.empty()) {
          callback(std::vector<DataValue>{});
          return;
        }

        state->monitored_items.reserve(read_value_ids.size());
        auto complete_one = [state, callback](size_t index,
                                              DataValue data_value) mutable {
          if (state->completed[index]) {
            return;
          }

          state->results[index] = std::move(data_value);
          state->completed[index] = true;
          if (--state->remaining == 0) {
            state->monitored_items.clear();
            callback(std::move(state->results));
          }
        };

        for (size_t i = 0; i < read_value_ids.size(); ++i) {
          auto monitored_item = monitored_item_service.CreateMonitoredItem(
              read_value_ids[i], monitoring_params);
          if (!monitored_item) {
            complete_one(i, MakeReadError(StatusCode::Bad_WrongNodeId));
            continue;
          }

          state->monitored_items.emplace_back(monitored_item);
          monitored_item->Subscribe(
              [complete_one, i](const DataValue& data_value) mutable {
                complete_one(i, data_value);
              });
        }
      });
}

}  // namespace scada
