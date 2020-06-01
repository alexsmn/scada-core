#pragma once

#include <functional>
#include <memory>
#include <vector>

#include "core/aggregate_filter.h"
#include "core/data_value.h"
#include "core/event.h"

namespace scada {

struct HistoryReadRawDetails {
  bool forward() const { return to.is_null() || from <= to; }

  NodeId node_id;
  base::Time from;
  base::Time to;
  size_t max_count = 0;
  AggregateFilter aggregation;
  bool release_continuation_point = false;
  ByteString continuation_point;
};

struct HistoryReadEventsDetails {
  scada::NodeId node_id;
  base::Time from;
  base::Time to;
  scada::EventFilter filter;
};

struct ItemInfo {
  NodeId node_id;
  DataValue data_value;
  base::Time change_time;
};

using ItemInfosCallback =
    std::function<void(std::vector<ItemInfo>&& item_infos)>;

struct HistoryReadRawResult {
  Status status;
  std::vector<DataValue> values;
  ByteString continuation_point;
};

using HistoryReadRawCallback =
    std::function<void(HistoryReadRawResult&& result)>;

using HistoryReadEventsCallback =
    std::function<void(Status&& status, std::vector<Event>&& events)>;

}  // namespace scada
