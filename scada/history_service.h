#pragma once

#include "base/awaitable.h"
#include "scada/event.h"
#include "scada/history_types.h"
#include "scada/status_or.h"

namespace scada {

class HistoryService {
 public:
  virtual ~HistoryService() = default;

  virtual Awaitable<StatusOr<HistoryReadRawResult>> HistoryReadRaw(
      HistoryReadRawDetails details) = 0;

  virtual Awaitable<HistoryReadEventsResult> HistoryReadEvents(
      NodeId node_id,
      scada::Time from,
      scada::Time to,
      EventFilter filter) = 0;
};

}  // namespace scada
