#include "scada/event.h"

#include <format>

#include <gtest/gtest.h>

namespace scada {
namespace {

TEST(ModelChangeEventTest, FormatNative) {
  ModelChangeEvent e;
  e.source_node_id = NodeId{33, 2};
  e.type_definition_id = NodeId{40, 0};
  e.set_verb(ModelChangeEvent::NodeAdded | ModelChangeEvent::ReferenceAdded);

  EXPECT_EQ(std::format("{}", e),
            "{node_id: \"ns=2;i=33\", type_definition_id: \"i=40\", "
            "verb: [\"NodeAdded\",\"ReferenceAdded\"]}");
}

TEST(ModelChangeEventTest, FormatEmptyVerb) {
  ModelChangeEvent e;
  e.source_node_id = NodeId{1, 0};

  EXPECT_EQ(std::format("{}", e),
            "{node_id: \"i=1\", type_definition_id: \"i=0\", verb: []}");
}

TEST(SemanticChangeEventTest, FormatNative) {
  SemanticChangeEvent e;
  e.source_node_id = NodeId{7, 1};

  EXPECT_EQ(std::format("{}", e), "{node_id: \"ns=1;i=7\"}");
}

}  // namespace
}  // namespace scada
