#include "scada/node_management_service_mock.h"

#include "base/test/awaitable_test.h"

#include <gmock/gmock.h>

using namespace testing;

namespace scada {
namespace {

// Every unstubbed call must hand back an awaitable that completes, and the
// per-item services keep their shape: one result per input. gmock's own
// fallback for `Awaitable<T>` is a null-frame awaitable that segfaults in the
// awaiting coroutine -- see CLAUDE.md, "Unit Test Guidance", and task 698.
TEST(NodeManagementServiceMockTest, EveryUnstubbedAwaitableCompletes) {
  TestExecutor executor;
  NiceMock<MockNodeManagementService> service;

  bool completed = false;
  WaitAwaitable(executor, [&]() -> Awaitable<void> {
    StatusOr<std::vector<AddNodesResult>> added = co_await service.AddNodes(
        ServiceContext{}, std::vector<AddNodesItem>(2));
    EXPECT_TRUE(added.has_value() && added->size() == 2);

    StatusOr<std::vector<StatusCode>> deleted = co_await service.DeleteNodes(
        ServiceContext{}, std::vector<DeleteNodesItem>(3));
    EXPECT_TRUE(deleted.has_value() && deleted->size() == 3);

    StatusOr<std::vector<StatusCode>> referenced =
        co_await service.AddReferences(ServiceContext{},
                                       std::vector<AddReferencesItem>(1));
    EXPECT_TRUE(referenced.has_value() && referenced->size() == 1);

    StatusOr<std::vector<StatusCode>> dereferenced =
        co_await service.DeleteReferences(ServiceContext{},
                                          std::vector<DeleteReferencesItem>{});
    EXPECT_TRUE(dereferenced.has_value() && dereferenced->empty());

    completed = true;
  }());

  EXPECT_TRUE(completed);
}

}  // namespace
}  // namespace scada
