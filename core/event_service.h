#pragma once

#include "base/containers/span.h"
#include "core/event.h"
#include "core/node_id.h"

namespace scada {

class EventService {
 public:
  virtual ~EventService() = default;

  virtual void Acknowledge(base::span<const EventAcknowledgeId> acknowledge_ids,
                           DateTime acknowledge_time,
                           const NodeId& user_id) = 0;
};

}  // namespace scada
