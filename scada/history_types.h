#pragma once

#include "base/struct_writer.h"
#include "scada/aggregate_filter.h"
#include "scada/data_value.h"
#include "scada/event.h"

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
  // Defines the root source node.
  NodeId node_id;
  base::Time from;
  base::Time to;
  EventFilter filter;
};

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

}  // namespace scada
