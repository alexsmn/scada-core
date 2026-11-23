#pragma once

#include "base/awaitable.h"
#include "scada/co_result.h"
#include "scada/event.h"
#include "scada/history_types.h"
#include "scada/service_context.h"
#include "scada/status_or.h"

namespace scada {

class HistoryService {
 public:
  virtual ~HistoryService() = default;

  // Reads the raw (or aggregated) value history of `details.node_id`.
  // `context` is the caller: a server answers only a caller holding the
  // ReadHistory permission on the node (OPC UA Part 3 §8.55 PermissionType
  // bit 7 ReadHistory,
  // https://reference.opcfoundation.org/Core/Part3/v105/docs/8.55). A
  // client-side implementation ignores it: the session it speaks through
  // carries the identity. A server-internal reader names itself with an
  // explicit context rather than passing a default one, which is anonymous.
  virtual CoStatusOr<HistoryReadRawResult> HistoryReadRaw(
      ServiceContext context,
      HistoryReadRawDetails details) = 0;

  // Reads the event history of `node_id`. `context` is the caller: a server
  // withholds every event the caller lacks the ReceiveEvents permission for
  // (OPC UA Part 3 §8.55 PermissionType bit 11,
  // https://reference.opcfoundation.org/Core/Part3/v105/docs/8.55), as it
  // does for live events, and resolves the events' Messages to the caller's
  // locale. A client-side implementation ignores it: the session it speaks
  // through carries the identity.
  virtual CoStatusOr<HistoryReadEventsResult> HistoryReadEvents(
      ServiceContext context,
      NodeId node_id,
      scada::Time from,
      scada::Time to,
      EventFilter filter) = 0;
};

}  // namespace scada
