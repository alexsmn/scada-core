#pragma once

#include "scada/history_service.h"

#include <gmock/gmock.h>

namespace scada {

class MockHistoryService : public HistoryService {
 public:
  MockHistoryService() {
    using namespace testing;
    ON_CALL(*this, HistoryReadRaw(_))
        .WillByDefault([](HistoryReadRawDetails) -> Awaitable<HistoryReadRawResult> {
          co_return HistoryReadRawResult{};
        });
    ON_CALL(*this, HistoryReadEvents(_, _, _, _))
        .WillByDefault([](NodeId, base::Time, base::Time,
                          EventFilter) -> Awaitable<HistoryReadEventsResult> {
          co_return HistoryReadEventsResult{};
        });
  }

  MOCK_METHOD(Awaitable<HistoryReadRawResult>,
              HistoryReadRaw,
              (HistoryReadRawDetails details),
              (override));

  MOCK_METHOD(Awaitable<HistoryReadEventsResult>,
              HistoryReadEvents,
              (NodeId node_id,
               base::Time from,
               base::Time to,
               EventFilter filter),
              (override));
};

}  // namespace scada
