#pragma once

#include "scada/co_result.h"
#include "scada/method_service.h"

#include <gmock/gmock.h>

namespace scada {

class MockMethodService : public MethodService {
 public:
  MOCK_METHOD(CoStatus,
              Call,
              (NodeId node_id,
               NodeId method_id,
               std::vector<Variant> arguments,
               ServiceContext context),
              (override));
};

inline CoStatus MakeMethodCallResult(Status status = StatusCode::Good) {
  co_return std::move(status);
}

}  // namespace scada
