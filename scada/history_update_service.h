#pragma once

#include "base/awaitable.h"
#include "scada/co_result.h"
#include "scada/history_types.h"
#include "scada/service_context.h"
#include "scada/status_or.h"

#include <vector>

namespace scada {

// The standard OPC UA write interface to a historian — the write counterpart of
// `HistoryService`. OPC UA Part 4 §5.10.5 HistoryUpdate,
// https://reference.opcfoundation.org/Core/Part4/v105/docs/5.10.5 and OPC UA
// Part 11 §6.8 HistoryUpdate,
// https://reference.opcfoundation.org/Core/Part11/v105/docs/6.8
//
// Implementations may be the local historian or a proxy to an external OPC UA
// historian; see docs/historian-opcua-pluggable-seam.md.
class HistoryUpdateService {
 public:
  virtual ~HistoryUpdateService() = default;

  // Inserts/replaces historical data values for a node (UpdateDataDetails).
  // Returns one StatusCode per supplied value; an operation-level failure is
  // reported via the StatusOr status.
  virtual CoStatusOr<std::vector<StatusCode>> HistoryUpdateData(
      ServiceContext context,
      UpdateDataDetails details) = 0;

  // Inserts historical events for a node (UpdateEventDetails). Returns one
  // StatusCode per supplied event; an operation-level failure is reported via
  // the StatusOr status. OPC UA Part 11 §6.8.4 UpdateEventDetails,
  // https://reference.opcfoundation.org/Core/Part11/v105/docs/6.8.4
  virtual CoStatusOr<std::vector<StatusCode>> HistoryUpdateEvent(
      ServiceContext context,
      UpdateEventDetails details) = 0;
};

}  // namespace scada
