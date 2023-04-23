#pragma once

#include "core/history_service.h"
#include "core/status_promise.h"

namespace scada {

inline promise<std::vector<Event>> HistoryReadEvents(
    HistoryService& history_service,
    const scada::NodeId& node_id,
    DateTime from,
    DateTime to,
    const EventFilter& event_filter = {}) {
  promise<std::vector<Event>> promise;
  history_service.HistoryReadEvents(
      node_id, from, to, event_filter,
      [promise](Status status, std::vector<Event> events) mutable {
        if (status.bad())
          promise.reject(StatusException{std::move(status)});
        else
          promise.resolve(std::move(events));
      });
  return promise;
}

}  // namespace scada