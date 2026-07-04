#pragma once

#include "scada/node_management_service.h"

#include <gmock/gmock.h>

namespace scada {

template <class T>
Awaitable<StatusOr<std::vector<T>>> MakeNodeManagementResult(
    StatusOr<std::vector<T>> result) {
  co_return std::move(result);
}

class MockNodeManagementService : public NodeManagementService {
 public:
  MOCK_METHOD((Awaitable<StatusOr<std::vector<AddNodesResult>>>),
              AddNodes,
              (ServiceContext context, std::vector<AddNodesItem> inputs),
              (override));

  MOCK_METHOD((Awaitable<StatusOr<std::vector<StatusCode>>>),
              DeleteNodes,
              (ServiceContext context, std::vector<DeleteNodesItem> inputs),
              (override));

  MOCK_METHOD((Awaitable<StatusOr<std::vector<StatusCode>>>),
              AddReferences,
              (ServiceContext context, std::vector<AddReferencesItem> inputs),
              (override));

  MOCK_METHOD((Awaitable<StatusOr<std::vector<StatusCode>>>),
              DeleteReferences,
              (ServiceContext context, std::vector<DeleteReferencesItem> inputs),
              (override));
};

}  // namespace scada
