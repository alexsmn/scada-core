#include "scada/view_service_mock.h"

#include "base/test/awaitable_test.h"

#include <gmock/gmock.h>

using namespace testing;

namespace scada {
namespace {

// Unstubbed browses complete with one empty result per input instead of
// gmock's null-frame awaitable, which segfaults the awaiting coroutine (task
// 698; CLAUDE.md, "Unit Test Guidance").
TEST(ViewServiceMockTest, EveryUnstubbedAwaitableCompletes) {
  TestExecutor executor;
  NiceMock<MockViewService> service;

  bool completed = false;
  WaitAwaitable(executor, [&]() -> Awaitable<void> {
    StatusOr<std::vector<BrowseResult>> browsed = co_await service.Browse(
        ServiceContext{}, std::vector<BrowseDescription>(2));
    EXPECT_TRUE(browsed.has_value() && browsed->size() == 2);

    StatusOr<std::vector<BrowsePathResult>> translated =
        co_await service.TranslateBrowsePaths(std::vector<BrowsePath>(1));
    EXPECT_TRUE(translated.has_value() && translated->size() == 1);

    completed = true;
  }());

  EXPECT_TRUE(completed);
}

}  // namespace
}  // namespace scada
