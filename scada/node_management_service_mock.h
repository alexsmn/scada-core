#pragma once

#include "scada/co_result.h"
#include "scada/node_management_service.h"

#include <gmock/gmock.h>

namespace scada {

template <class T>
CoStatusOr<std::vector<T>> MakeNodeManagementResult(
    StatusOr<std::vector<T>> result) {
  co_return std::move(result);
}

class MockNodeManagementService : public NodeManagementService {
 public:
  // An unstubbed `Awaitable<T>` return is gmock's `T()`: a null-frame
  // awaitable whose `await_ready()` still answers false, so `co_await` on it
  // segfaults inside the AWAITING coroutine, naming no mock. Every awaitable
  // method therefore completes by default with the emptiest honest answer.
  // See CLAUDE.md, "Unit Test Guidance"; the `*_mock_unittest.cpp` beside
  // this header pins it.
  MockNodeManagementService() {
    ON_CALL(*this, AddNodes)
        .WillByDefault([](ServiceContext, std::vector<AddNodesItem> inputs)
                           -> CoStatusOr<std::vector<AddNodesResult>> {
          co_return std::vector<AddNodesResult>(inputs.size());
        });
    ON_CALL(*this, DeleteNodes)
        .WillByDefault([](ServiceContext, std::vector<DeleteNodesItem> inputs)
                           -> CoStatusOr<std::vector<StatusCode>> {
          co_return std::vector<StatusCode>(inputs.size());
        });
    ON_CALL(*this, AddReferences)
        .WillByDefault([](ServiceContext, std::vector<AddReferencesItem> inputs)
                           -> CoStatusOr<std::vector<StatusCode>> {
          co_return std::vector<StatusCode>(inputs.size());
        });
    ON_CALL(*this, DeleteReferences)
        .WillByDefault(
            [](ServiceContext, std::vector<DeleteReferencesItem> inputs)
                -> CoStatusOr<std::vector<StatusCode>> {
              co_return std::vector<StatusCode>(inputs.size());
            });
  }

  MOCK_METHOD((CoStatusOr<std::vector<AddNodesResult>>),
              AddNodes,
              (ServiceContext context, std::vector<AddNodesItem> inputs),
              (override));

  MOCK_METHOD((CoStatusOr<std::vector<StatusCode>>),
              DeleteNodes,
              (ServiceContext context, std::vector<DeleteNodesItem> inputs),
              (override));

  MOCK_METHOD((CoStatusOr<std::vector<StatusCode>>),
              AddReferences,
              (ServiceContext context, std::vector<AddReferencesItem> inputs),
              (override));

  MOCK_METHOD((CoStatusOr<std::vector<StatusCode>>),
              DeleteReferences,
              (ServiceContext context,
               std::vector<DeleteReferencesItem> inputs),
              (override));
};

}  // namespace scada
