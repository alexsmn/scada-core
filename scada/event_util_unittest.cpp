#include "scada/event_util.h"

#include "scada/event.h"
#include "scada/standard_node_ids.h"

#include <gtest/gtest.h>

namespace scada {
namespace {

Event MakeEvent(NodeId event_type_id) {
  Event event;
  event.event_type_id = std::move(event_type_id);
  event.event_id = 0x1234567;
  event.time = DateTime::Now();
  event.receive_time = DateTime::Now();
  event.change_mask = Event::EVT_VAL;
  event.severity = kSeverityWarning;
  event.source_node_id = NodeId{42, 2};
  event.user_id = NodeId{7, 3};
  event.value = Variant{123};
  event.message = u"message";
  return event;
}

TEST(EventUtilTest, SystemEventRoundTrips) {
  const Event event = MakeEvent(NodeId{id::SystemEventType});

  const std::vector<Variant> fields = DisassembleEvent(std::any{event});
  const std::any assembled = AssembleEvent(fields);

  const auto* result = std::any_cast<Event>(&assembled);
  ASSERT_NE(result, nullptr);
  EXPECT_EQ(*result, event);
}

TEST(EventUtilTest, NonCoreEventTypeRoundTripsAsEvent) {
  // Subtypes and model-owned event types (e.g. DeviceWatchEventType) share
  // the scada::Event layout; assembly dispatches on the layout, not on an
  // enumerated type-id list, so core does not need to name them.
  const Event event = MakeEvent(NodeId{15132, 1});

  const std::any assembled = AssembleEvent(DisassembleEvent(std::any{event}));

  const auto* result = std::any_cast<Event>(&assembled);
  ASSERT_NE(result, nullptr);
  EXPECT_EQ(*result, event);
}

TEST(EventUtilTest, ModelChangeEventRoundTrips) {
  ModelChangeEvent event;
  event.node_id = NodeId{5, 2};
  event.type_definition_id = NodeId{6, 2};
  event.verb = ModelChangeEvent::NodeAdded;

  const std::any assembled = AssembleEvent(DisassembleEvent(std::any{event}));

  const auto* result = std::any_cast<ModelChangeEvent>(&assembled);
  ASSERT_NE(result, nullptr);
  EXPECT_EQ(*result, event);
}

TEST(EventUtilTest, SemanticChangeEventRoundTrips) {
  SemanticChangeEvent event;
  event.node_id = NodeId{5, 2};

  const std::any assembled = AssembleEvent(DisassembleEvent(std::any{event}));

  const auto* result = std::any_cast<SemanticChangeEvent>(&assembled);
  ASSERT_NE(result, nullptr);
  EXPECT_EQ(*result, event);
}

TEST(EventUtilTest, MalformedFieldsAssembleEmpty) {
  // Wire data must never panic: a recognized-looking payload with the wrong
  // field count (or a null type id) degrades to an empty event.
  const std::vector<Variant> wrong_count{Variant{NodeId{id::SystemEventType}},
                                         Variant{u"x"}};
  EXPECT_FALSE(AssembleEvent(wrong_count).has_value());

  const std::vector<Variant> null_type{Variant{}};
  EXPECT_FALSE(AssembleEvent(null_type).has_value());
}

}  // namespace
}  // namespace scada
