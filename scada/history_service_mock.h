#pragma once

#include "scada/history_service.h"

#include <gmock/gmock.h>

namespace scada {

class MockHistoryService : public HistoryService {
 public:
  MockHistoryService() {
    using namespace testing;
    ON_CALL(*this, HistoryReadRaw(_))
        .WillByDefault([](HistoryReadRawDetails)
                           -> Awaitable<StatusOr<HistoryReadRawResult>> {
          co_return HistoryReadRawResult{};
        });
    ON_CALL(*this, HistoryReadEvents(_, _, _, _))
        .WillByDefault(
            [](NodeId, scada::Time, scada::Time,
               EventFilter) -> Awaitable<StatusOr<HistoryReadEventsResult>> {
              co_return HistoryReadEventsResult{};
            });
  }

  MOCK_METHOD(Awaitable<StatusOr<HistoryReadRawResult>>,
              HistoryReadRaw,
              (HistoryReadRawDetails details),
              (override));

  MOCK_METHOD(
      Awaitable<StatusOr<HistoryReadEventsResult>>,
      HistoryReadEvents,
      (NodeId node_id, scada::Time from, scada::Time to, EventFilter filter),
      (override));
};

}  // namespace scada
