#include "scada/session_service_mock.h"

#include "base/test/awaitable_test.h"

#include <gmock/gmock.h>

using namespace testing;

namespace scada {
namespace {

// `Connect`, `Reconnect` and `Disconnect` were guarded on 2026-08-31;
// `ConnectStatus` was the one awaitable left returning gmock's null-frame
// awaitable, which segfaults the awaiting coroutine (task 698; CLAUDE.md,
// "Unit Test Guidance").
TEST(SessionServiceMockTest, EveryUnstubbedAwaitableCompletes) {
  TestExecutor executor;
  NiceMock<MockSessionService> service;

  bool completed = false;
  WaitAwaitable(executor, [&]() -> Awaitable<void> {
    co_await service.Connect(SessionConnectParams{});
    EXPECT_TRUE(
        (co_await service.ConnectStatus(SessionConnectParams{})).good());
    co_await service.Reconnect();
    co_await service.Disconnect();
    completed = true;
  }());

  EXPECT_TRUE(completed);
}

}  // namespace
}  // namespace scada
