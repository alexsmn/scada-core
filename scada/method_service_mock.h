#pragma once

#include "scada/co_result.h"
#include "scada/method_service.h"

#include <gmock/gmock.h>

namespace scada {

class MockMethodService : public MethodService {
 public:
  // An unstubbed `Awaitable<T>` return is gmock's `T()`: a null-frame
  // awaitable whose `await_ready()` still answers false, so `co_await` on it
  // segfaults inside the AWAITING coroutine, naming no mock. Every awaitable
  // method therefore completes by default with the emptiest honest answer.
  // See CLAUDE.md, "Unit Test Guidance"; the `*_mock_unittest.cpp` beside
  // this header pins it.
  MockMethodService() {
    ON_CALL(*this, Call)
        .WillByDefault([](NodeId, NodeId, std::vector<Variant>,
                          ServiceContext) -> CoStatusOr<CallResult> {
          co_return StatusCode::Bad_NotSupported;
        });
  }

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
