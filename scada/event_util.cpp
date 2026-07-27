#include "scada/event_util.h"

#include "base/check.h"
#include "scada/event.h"

namespace scada {

// Field count of the scada::Event wire layout (see DisassembleEvent). The
// layout serves every scada::Event-carried type (SystemEventType and its
// subtypes, DeviceWatchEventType), so assembly dispatches on it rather than
// on an enumerated type-id list. The pre-ADR-0005 layout had no source_name
// (14 fields); assembly accepts both counts for the rollout window.
constexpr size_t kEventFieldCount = 15;
constexpr size_t kEventFieldCountV1 = 14;

// Field count of the DeviceFrameEvent layout: the base event's fields followed
// by the eight frame fields. It is its own layout rather than an extension of
// the base one so that adding frame data costs nothing on every other event —
// alarms, model changes and journal history keep the 15-field layout and stay
// wire-compatible. Dispatch is by count, as it is for the model-notification
// layouts; the count must stay distinct from 2, 4, 14 and 15.
constexpr size_t kDeviceFrameFieldCount = kEventFieldCount + 8;

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
  if (fields.size() > 14) {
    fields[14].get(event.source_name);
  }
  return event;
}

scada::DeviceFrameEvent AssembleDeviceFrameEvent(
    std::span<const scada::Variant> fields) {
  scada::DeviceFrameEvent event;
  event.base = AssembleBaseEvent(fields.first(kEventFieldCount));
  fields[15].get(event.direction);
  fields[16].get(event.raw_data);
  fields[17].get(event.format);
  fields[18].get(event.type_id);
  fields[19].get(event.cause);
  fields[20].get(event.object_address);
  fields[21].get(event.send_sequence);
  fields[22].get(event.receive_sequence);
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
  } else if (!event_type_id.is_null() &&
             fields.size() == kDeviceFrameFieldCount) {
    return AssembleDeviceFrameEvent(fields);
  } else if (!event_type_id.is_null() && (fields.size() == kEventFieldCount ||
                                          fields.size() == kEventFieldCountV1)) {
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
      event.source_name,
  };
}

std::vector<scada::Variant> DisassembleEvent(
    const scada::DeviceFrameEvent& event) {
  // The base layout verbatim, then the frame fields. Keeping the base prefix
  // intact means AssembleBaseEvent reads it unchanged and a frame event
  // degrades cleanly to its log line for anything that only wants that.
  std::vector<scada::Variant> fields = DisassembleEvent(event.base);
  fields.push_back(event.direction);
  fields.push_back(event.raw_data);
  fields.push_back(event.format);
  fields.push_back(event.type_id);
  fields.push_back(event.cause);
  fields.push_back(event.object_address);
  fields.push_back(event.send_sequence);
  fields.push_back(event.receive_sequence);
  return fields;
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
  } else if (auto* device_frame_event =
                 std::any_cast<scada::DeviceFrameEvent>(&event)) {
    return DisassembleEvent(*device_frame_event);
  } else {
    base::NotReached();
    return {};
  }
}

}  // namespace scada
