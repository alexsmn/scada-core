#pragma once

#include "core/history_service.h"
#include "core/status_promise.h"

namespace scada {

inline promise<HistoryReadRawResult> HistoryReadRaw(
    HistoryService& history_service,
    const HistoryReadRawDetails& details) {
  promise<HistoryReadRawResult> promise;
  history_service.HistoryReadRaw(
      details, [promise](HistoryReadRawResult&& result) mutable {
        if (result.status)
          promise.resolve(std::move(result));
        else
          RejectStatusPromise(promise, std::move(result.status));
      });
  return promise;
}

inline promise<std::vector<Event>> HistoryReadEvents(
    HistoryService& history_service,
    const NodeId& node_id,
    DateTime from,
    DateTime to,
    const EventFilter& event_filter = {}) {
  promise<std::vector<Event>> promise;
  history_service.HistoryReadEvents(
      node_id, from, to, event_filter,
      [promise](Status status, std::vector<Event> events) mutable {
        if (status)
          promise.resolve(std::move(events));
        else
          RejectStatusPromise(promise, std::move(status));
      });
  return promise;
}

}  // namespace scada
