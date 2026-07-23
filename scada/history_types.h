#pragma once

#include "scada/aggregate_filter.h"
#include "scada/data_value.h"
#include "scada/event.h"
#include "scada/event_filter.h"

#include <functional>
#include <memory>
#include <vector>

namespace scada {

struct HistoryReadRawDetails {
  bool forward() const { return scada::IsNull(to) || from <= to; }

  NodeId node_id;
  scada::DateTime from = scada::kNullTime;
  scada::DateTime to = scada::kNullTime;
  size_t max_count = 0;
  AggregateFilter aggregation;
  bool release_continuation_point = false;
  ByteString continuation_point;
};

struct HistoryReadEventsDetails {
  // Defines the root source node.
  NodeId node_id;
  scada::DateTime from = scada::kNullTime;
  scada::DateTime to = scada::kNullTime;
  EventFilter filter;
};

struct HistoryReadRawResult {
  Status status{StatusCode::Good};
  std::vector<DataValue> values;
  ByteString continuation_point;
};

struct HistoryReadEventsResult {
  Status status{StatusCode::Good};
  std::vector<Event> events;
};

// How a HistoryUpdate applies the supplied values.
// OPC UA Part 11 §6.8.3 PerformUpdateType,
// https://reference.opcfoundation.org/Core/Part11/v105/docs/6.8.3
enum class PerformUpdateType {
  Insert = 1,   // Insert; error if a value already exists at the timestamp.
  Replace = 2,  // Replace; error if no value exists at the timestamp.
  Update = 3,   // Insert or replace.
  Remove = 4,
};

// Insert/replace historical data values for a node.
// OPC UA Part 11 §6.8.2 UpdateDataDetails,
// https://reference.opcfoundation.org/Core/Part11/v105/docs/6.8.2
struct UpdateDataDetails {
  NodeId node_id;
  PerformUpdateType perform_insert_replace = PerformUpdateType::Update;
  std::vector<DataValue> values;
};

// Insert historical events for a node.
// OPC UA Part 11 §6.8.4 UpdateEventDetails,
// https://reference.opcfoundation.org/Core/Part11/v105/docs/6.8.4
struct UpdateEventDetails {
  NodeId node_id;
  PerformUpdateType perform_insert_replace = PerformUpdateType::Insert;
  std::vector<Event> events;
};

using AcknowledgeCallback =
    std::function<void(Status status, std::vector<StatusCode> results)>;

}  // namespace scada
