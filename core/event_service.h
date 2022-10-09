#pragma once

#include "base/containers/span.h"
#include "core/event.h"
#include "core/node_id.h"

namespace scada {

class EventService {
 public:
  virtual ~EventService() {}

  virtual void Acknowledge(
      base::span<const scada::EventAcknowledgeId> acknowledge_ids,
      const NodeId& user_id) = 0;
};

}  // namespace scada
