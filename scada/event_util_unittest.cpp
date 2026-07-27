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
  event.time = scada::Now();
  event.receive_time = scada::Now();
  event.change_mask = Event::EVT_VAL;
  event.severity = kSeverityWarning;
  event.source_node_id = NodeId{42, 2};
  event.source_name = "Pump 42";
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

TEST(EventUtilTest, LegacyFourteenFieldLayoutAssemblesWithoutSourceName) {
  // Pre-ADR-0005 peers send the 14-field layout (no source_name); assembly
  // must accept it during the rollout window.
  const Event event = MakeEvent(NodeId{id::SystemEventType});
  std::vector<Variant> fields = DisassembleEvent(std::any{event});
  ASSERT_EQ(fields.size(), 15u);
  fields.pop_back();

  const std::any assembled = AssembleEvent(fields);
  const auto* result = std::any_cast<Event>(&assembled);
  ASSERT_NE(result, nullptr);
  EXPECT_TRUE(result->source_name.empty());
  Event expected = event;
  expected.source_name.clear();
  EXPECT_EQ(*result, expected);
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

DeviceFrameEvent MakeFrameEvent() {
  DeviceFrameEvent event;
  // The producing tier supplies the type id: DeviceFrameEventType lives in the
  // SCADA model namespace, which core does not depend on.
  event.base = MakeEvent(NodeId{15133, 7});
  event.base.message = u"#RX: 68 0C FA 0F";
  event.frame.direction = DeviceFrame::kInbound;
  event.frame.raw_data = ByteString{0x68, 0x0c, '\xfa', '\x0f'};
  event.frame.format = "I";
  event.frame.type_id = 13;
  event.frame.cause = 1;
  event.frame.object_address = 4002;
  event.frame.send_sequence = 2045;
  event.frame.receive_sequence = 1602;
  return event;
}

TEST(EventUtilTest, DeviceFrameEventRoundTrips) {
  const DeviceFrameEvent event = MakeFrameEvent();

  const std::vector<Variant> fields = DisassembleEvent(std::any{event});
  const std::any assembled = AssembleEvent(fields);

  const auto* result = std::any_cast<DeviceFrameEvent>(&assembled);
  ASSERT_NE(result, nullptr) << "frame layout did not assemble as a frame";
  EXPECT_EQ(*result, event);
}

// The frame layout must not disturb the base one: its whole reason for existing
// is that every other event keeps the 15-field layout and stays wire
// compatible.
TEST(EventUtilTest, DeviceFrameLayoutIsDistinctFromTheBaseLayout) {
  const std::vector<Variant> frame_fields =
      DisassembleEvent(std::any{MakeFrameEvent()});
  const std::vector<Variant> base_fields =
      DisassembleEvent(std::any{MakeEvent(NodeId{id::SystemEventType})});

  EXPECT_EQ(base_fields.size(), 15u);
  EXPECT_EQ(frame_fields.size(), 23u);

  // The base prefix is carried verbatim, so a frame degrades cleanly to its
  // log line for anything that only reads the base event.
  const std::any as_base =
      AssembleEvent(std::span{frame_fields}.first(base_fields.size()));
  const auto* base = std::any_cast<Event>(&as_base);
  ASSERT_NE(base, nullptr);
  EXPECT_EQ(base->message.text, u"#RX: 68 0C FA 0F");
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
