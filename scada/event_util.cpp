#include "scada/event_util.h"

#include "base/check.h"
#include "scada/event.h"

namespace scada {

// Field count of the scada::Event wire layout (see DisassembleEvent). The
// layout serves every scada::Event-carried type (SystemEventType and its
// subtypes, DeviceWatchEventType), so assembly dispatches on it rather than
// on an enumerated type-id list.
constexpr size_t kEventFieldCount = 14;

scada::Event AssembleBaseEvent(std::span<const scada::Variant> fields) {
  scada::Event event;
  fields[0].get(event.event_type_id);
  fields[1].get(event.event_id);
  fields[2].get(event.time);
  fields[3].get(event.change_mask);
  fields[4].get(event.severity);
  fields[5].get(event.source_node_id);
  fields[6].get(event.user_id);
  event.value = fields[7];
  event.qualifier = scada::Qualifier{fields[8].get_or<unsigned>(0)};
  fields[9].get(event.message);
  fields[10].get(event.acked);
  fields[11].get(event.acknowledged_time);
  fields[12].get(event.acknowledged_user_id);
  fields[13].get(event.receive_time);
  return event;
}

scada::ModelChangeEvent AssembleModelChangeEvent(
    std::span<const scada::Variant> fields) {
  scada::ModelChangeEvent event;
  fields[1].get(event.node_id);
  fields[2].get(event.type_definition_id);
  fields[3].get(event.verb);
  return event;
}

scada::SemanticChangeEvent AssembleSemanticChangeEvent(
    std::span<const scada::Variant> fields) {
  scada::SemanticChangeEvent event;
  fields[1].get(event.node_id);
  return event;
}

std::any AssembleEvent(std::span<const scada::Variant> fields) {
  base::Check(!fields.empty());
  if (fields.empty())
    return {};

  // Every layout carries the event type id at field 0 (see the
  // DisassembleEvent overloads). The two model-notification types have
  // dedicated layouts; any other type id with the scada::Event field count
  // assembles as a scada::Event, so subtypes (and DeviceWatchEventType,
  // which core cannot name) survive the wire without core enumerating them.
  // Field counts are checked before indexing — the fields arrive from the
  // wire, and a malformed layout must degrade to an empty event, not panic
  // or read out of bounds.
  auto event_type_id = fields[0].get_or<scada::NodeId>({});
  if (event_type_id == scada::id::GeneralModelChangeEventType &&
      fields.size() == 4) {
    return AssembleModelChangeEvent(fields);
  } else if (event_type_id == scada::id::SemanticChangeEventType &&
             fields.size() == 2) {
    return AssembleSemanticChangeEvent(fields);
  } else if (!event_type_id.is_null() && fields.size() == kEventFieldCount) {
    return AssembleBaseEvent(fields);
  } else {
    return {};
  }
}

std::vector<scada::Variant> DisassembleEvent(const scada::Event& event) {
  // Field 0 is the event type id in EVERY layout — AssembleEvent dispatches
  // on it. (It historically held event_id here, which made system events
  // unassemblable on the receiving side; both bridge ends ship together, so
  // the layouts stay symmetric within a deployment.)
  return {
      event.event_type_id,
      event.event_id,
      event.time,
      event.change_mask,
      event.severity,
      event.source_node_id,
      event.user_id,
      event.value,
      event.qualifier.raw(),
      event.message,
      event.acked,
      event.acknowledged_time,
      event.acknowledged_user_id,
      event.receive_time,
  };
}

std::vector<scada::Variant> DisassembleEvent(
    const scada::ModelChangeEvent& event) {
  return {
      scada::NodeId{event.event_type_id},
      event.node_id,
      event.type_definition_id,
      event.verb,
  };
}

std::vector<scada::Variant> DisassembleEvent(
    const scada::SemanticChangeEvent& event) {
  return {
      scada::NodeId{event.event_type_id},
      event.node_id,
  };
}

std::vector<scada::Variant> DisassembleEvent(const std::any& event) {
  base::Check(event.has_value());
  if (auto* system_event = std::any_cast<scada::Event>(&event)) {
    return DisassembleEvent(*system_event);
  } else if (auto* model_change_event =
                 std::any_cast<scada::ModelChangeEvent>(&event)) {
    return DisassembleEvent(*model_change_event);
  } else if (auto* semantic_change_event =
                 std::any_cast<scada::SemanticChangeEvent>(&event)) {
    return DisassembleEvent(*semantic_change_event);
  } else {
    base::NotReached();
    return {};
  }
}

}  // namespace scada
