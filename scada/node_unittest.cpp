#include "scada/node.h"

#include "base/test/awaitable_test.h"
#include "base/test/test_executor.h"
#include "scada/client.h"
#include "scada/co_result.h"

#include <gtest/gtest.h>

namespace scada {
namespace {

// Records the browse descriptions it receives and returns a single reference
// per input, pointing at `target_id`.
class FakeViewService : public ViewService {
 public:
  CoStatusOr<std::vector<BrowseResult>> Browse(
      ServiceContext context,
      std::vector<BrowseDescription> inputs) override {
    browse_inputs = inputs;
    std::vector<BrowseResult> results;
    for (const auto& input : inputs) {
      results.push_back(BrowseResult{
          .references = {
              {.reference_type_id = input.reference_type_id,
               .forward = input.direction == BrowseDirection::Forward,
               .node_id = target_id}}});
    }
    co_return results;
  }

  CoStatusOr<std::vector<BrowsePathResult>> TranslateBrowsePaths(
      std::vector<BrowsePath> inputs) override {
    co_return StatusCode::Bad_NotSupported;
  }

  NodeId target_id{42, 1};
  std::vector<BrowseDescription> browse_inputs;
};

class NodeTest : public testing::Test {
 protected:
  TestExecutor executor_;
  FakeViewService view_service_;
  client client_{services{.view_service = &view_service_}};

  inline static const NodeId node_id{7, 1};
};

// Regression test for a CP.53 dangling-parameter bug: `parent()` and
// `type_definition()` are plain (non-coroutine) wrappers that hand a braced
// `browse_details` temporary to the lazy `browse_node` coroutine and return
// the awaitable. When `browse_details` was taken by const&, the temporary was
// destroyed as the wrapper returned, and the coroutine body read a dangling
// reference once resumed. `StartAwaitable` reproduces the deferred shape: the
// awaitable is created first and only awaited later from a spawned coroutine.
TEST_F(NodeTest, ParentBrowsesHierarchicalReferencesInverse) {
  // The node must outlive the awaitable: member coroutines capture `this`,
  // so a `client_.node(...)` temporary would dangle under this deferred
  // shape (see the lifetime comment in node.h).
  const scada::node node = client_.node(node_id);
  auto result = StartAwaitable(executor_, node.parent());
  auto parent = WaitResult(executor_, std::move(result));

  ASSERT_TRUE(parent.ok()) << parent.status();
  EXPECT_EQ(parent->id(), view_service_.target_id);

  ASSERT_EQ(view_service_.browse_inputs.size(), 1u);
  const BrowseDescription& input = view_service_.browse_inputs.front();
  EXPECT_EQ(input.node_id, node_id);
  EXPECT_EQ(input.reference_type_id, id::HierarchicalReferences);
  EXPECT_EQ(input.direction, BrowseDirection::Inverse);
}

TEST_F(NodeTest, TypeDefinitionBrowsesHasTypeDefinitionForward) {
  const scada::node node = client_.node(node_id);
  auto result = StartAwaitable(executor_, node.type_definition());
  auto type_definition = WaitResult(executor_, std::move(result));

  ASSERT_TRUE(type_definition.ok()) << type_definition.status();
  EXPECT_EQ(type_definition->id(), view_service_.target_id);

  ASSERT_EQ(view_service_.browse_inputs.size(), 1u);
  const BrowseDescription& input = view_service_.browse_inputs.front();
  EXPECT_EQ(input.node_id, node_id);
  EXPECT_EQ(input.reference_type_id, id::HasTypeDefinition);
  EXPECT_EQ(input.direction, BrowseDirection::Forward);
}

}  // namespace
}  // namespace scada
