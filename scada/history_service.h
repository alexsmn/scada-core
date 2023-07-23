#pragma once

#include "scada/event.h"
#include "scada/history_types.h"

namespace scada {

class HistoryService {
 public:
  virtual ~HistoryService() {}

  virtual void HistoryReadRaw(const HistoryReadRawDetails& details,
                              const HistoryReadRawCallback& callback) = 0;

  virtual void HistoryReadEvents(const NodeId& node_id,
                                 base::Time from,
                                 base::Time to,
                                 const EventFilter& filter,
                                 const HistoryReadEventsCallback& callback) = 0;
};

}  // namespace scada
