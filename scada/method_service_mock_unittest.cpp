#include "scada/method_service_mock.h"

#include "base/test/awaitable_test.h"

#include <gmock/gmock.h>

using namespace testing;

namespace scada {
namespace {

// An unstubbed `Call` completes with a bad status rather than with gmock's
// null-frame awaitable, which would segfault the awaiting coroutine (task
// 698; CLAUDE.md, "Unit Test Guidance").
TEST(MethodServiceMockTest, UnstubbedCallCompletesWithABadStatus) {
  TestExecutor executor;
  NiceMock<MockMethodService> service;

  StatusOr<CallResult> result = WaitAwaitable(
      executor, service.Call(NodeId{1}, NodeId{2}, {}, ServiceContext{}));

  EXPECT_FALSE(result.has_value());
}

}  // namespace
}  // namespace scada
