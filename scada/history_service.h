#pragma once

#include "base/awaitable.h"
#include "scada/event.h"
#include "scada/history_types.h"

namespace scada {

class HistoryService {
 public:
  virtual ~HistoryService() = default;

  virtual Awaitable<HistoryReadRawResult> HistoryReadRaw(
      HistoryReadRawDetails details) = 0;

  virtual Awaitable<HistoryReadEventsResult> HistoryReadEvents(
      NodeId node_id,
      base::Time from,
      base::Time to,
      EventFilter filter) = 0;
};

}  // namespace scada
