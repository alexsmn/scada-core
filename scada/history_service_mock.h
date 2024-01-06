#pragma once

#include "scada/history_service.h"

#include <gmock/gmock.h>

namespace scada {

class MockHistoryService : public HistoryService {
 public:
  MockHistoryService() {
    using namespace testing;
    ON_CALL(*this, HistoryReadRaw(_, _))
        .WillByDefault(InvokeArgument<1>(scada::HistoryReadRawResult{}));
    ON_CALL(*this, HistoryReadEvents(_, _, _, _, _))
        .WillByDefault(InvokeArgument<4>(scada::HistoryReadEventsResult{}));
  }

  MOCK_METHOD(void,
              HistoryReadRaw,
              (const HistoryReadRawDetails& details,
               const HistoryReadRawCallback& callback),
              (override));

  MOCK_METHOD(void,
              HistoryReadEvents,
              (const NodeId& node_id,
               base::Time from,
               base::Time to,
               const EventFilter& filter,
               const HistoryReadEventsCallback& callback),
              (override));
};

}  // namespace scada
