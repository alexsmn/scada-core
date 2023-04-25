#pragma once

#include "core/event_service.h"

#include <gmock/gmock.h>

namespace scada {

class MockEventService : public EventService {
 public:
  MOCK_METHOD(void,
              Acknowledge,
              (base::span<const EventAcknowledgeId> acknowledge_ids,
               DateTime acknowledge_time,
               const NodeId& user_id),
              (override));
};

}  // namespace scada
