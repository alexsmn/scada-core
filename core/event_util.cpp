#include "core/event_util.h"

#include "core/event.h"

namespace scada {

scada::Event AssembleSystemEvent(base::span<const scada::Variant> fields) {
  scada::Event event;
  fields[0].get(event.event_type_id);
  fields[1].get(event.time);
  fields[2].get(event.change_mask);
  fields[3].get(event.severity);
  fields[4].get(event.node_id);
  fields[5].get(event.user_id);
  event.value = fields[6];
  event.qualifier = scada::Qualifier{fields[7].get_or<unsigned>(0)};
  fields[8].get(event.message);
  fields[9].get(event.acked);
  fields[10].get(event.acknowledge_id);
  fields[11].get(event.acknowledged_time);
  fields[12].get(event.acknowledged_user_id);
  return event;
}

scada::ModelChangeEvent AssembleModelChangeEvent(
    base::span<const scada::Variant> fields) {
  scada::ModelChangeEvent event;
  fields[1].get(event.node_id);
  fields[2].get(event.type_definition_id);
  fields[3].get(event.verb);
  return event;
}

scada::SemanticChangeEvent AssembleSemanticChangeEvent(
    base::span<const scada::Variant> fields) {
  scada::SemanticChangeEvent event;
  fields[1].get(event.node_id);
  return event;
}

std::any AssembleEvent(base::span<const scada::Variant> fields) {
  assert(!fields.empty());
  if (fields.empty())
    return {};

  auto event_type_id = fields[0].get_or<scada::NodeId>({});
  if (event_type_id == scada::id::SystemEventType) {
    return AssembleSystemEvent(fields);
  } else if (event_type_id == scada::id::GeneralModelChangeEventType) {
    return AssembleModelChangeEvent(fields);
  } else if (event_type_id == scada::id::SemanticChangeEventType) {
    return AssembleSemanticChangeEvent(fields);
  } else {
    assert(false);
    return {};
  }
}

std::vector<scada::Variant> DisassembleEvent(const scada::Event& event) {
  return {
      event.event_type_id,
      event.time,
      event.change_mask,
      event.severity,
      event.node_id,
      event.user_id,
      event.value,
      event.qualifier.raw(),
      event.message,
      event.acked,
      event.acknowledge_id,
      event.acknowledged_time,
      event.acknowledged_user_id,
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
  assert(event.has_value());
  if (auto* system_event = std::any_cast<scada::Event>(&event)) {
    return DisassembleEvent(*system_event);
  } else if (auto* model_change_event =
                 std::any_cast<scada::ModelChangeEvent>(&event)) {
    return DisassembleEvent(*model_change_event);
  } else if (auto* semantic_change_event =
                 std::any_cast<scada::SemanticChangeEvent>(&event)) {
    return DisassembleEvent(*semantic_change_event);
  } else {
    assert(false);
    return {};
  }
}

}  // namespace scada
