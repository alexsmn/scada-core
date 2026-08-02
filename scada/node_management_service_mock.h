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
