#pragma once

#include "scada/co_result.h"
#include "scada/method_service.h"

#include <gmock/gmock.h>

namespace scada {

class MockMethodService : public MethodService {
 public:
  MOCK_METHOD(CoStatusOr<CallResult>,
              Call,
              (NodeId node_id,
               NodeId method_id,
               std::vector<Variant> arguments,
               ServiceContext context),
              (override));
};

inline CoStatusOr<CallResult> MakeMethodCallResult(
    Status status = StatusCode::Good) {
  co_return MakeCallResult(std::move(status));
}

// A successful call returning output arguments.
inline CoStatusOr<CallResult> MakeMethodCallResult(
    std::vector<Variant> outputs) {
  co_return CallResult{std::move(outputs)};
}

}  // namespace scada
