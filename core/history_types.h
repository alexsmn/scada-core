#pragma once

#include "base/struct_writer.h"
#include "core/aggregate_filter.h"
#include "core/data_value.h"
#include "core/event.h"

#include <functional>
#include <memory>
#include <optional>
#include <vector>

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
  NodeId node_id;
  base::Time from;
  base::Time to;
  EventFilter filter;
};

struct ItemInfo {
  NodeId node_id;
  DataValue data_value;
  base::Time change_time;
};

using ItemInfosCallback = std::function<void(std::vector<ItemInfo> item_infos)>;

using HistoryReadItemInfoCallback =
    std::function<void(std::optional<ItemInfo> item_info)>;

struct HistoryReadRawResult {
  Status status{StatusCode::Good};
  std::vector<DataValue> values;
  ByteString continuation_point;
};

using HistoryReadRawCallback = std::function<void(HistoryReadRawResult result)>;

struct HistoryReadEventsResult {};

using HistoryReadEventsCallback =
    std::function<void(Status status, std::vector<Event> events)>;

using AcknowledgeCallback =
    std::function<void(Status status, std::vector<StatusCode> results)>;

inline bool operator==(const ItemInfo& a, const ItemInfo& b) {
  return std::tie(a.node_id, a.data_value, a.change_time) ==
         std::tie(b.node_id, b.data_value, b.change_time);
}

inline std::ostream& operator<<(std::ostream& stream, const ItemInfo& x) {
  StructWriter{stream}
      .AddField("node_id", x.node_id)
      .AddField("data_value", x.data_value)
      .AddField("change_time", x.change_time);
  return stream;
}

}  // namespace scada
