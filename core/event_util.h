#pragma once

#include "base/containers/span.h"
#include "core/event.h"
#include "core/variant.h"

#include <any>

namespace scada {

inline scada::Event AssembleSystemEvent(
    base::span<const scada::Variant> fields) {
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

inline std::any AssembleEvent(base::span<const scada::Variant> fields) {
  assert(!fields.empty());
  if (fields.empty())
    return {};

  auto event_type_id = fields[0].get_or<scada::NodeId>({});
  return AssembleSystemEvent(fields);
}

inline std::vector<scada::Variant> DisassembleEvent(const scada::Event& event) {
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

inline std::vector<scada::Variant> DisassembleEvent(const std::any& event) {
  assert(event.has_value());
  if (auto* system_event = std::any_cast<scada::Event>(&event)) {
    return DisassembleEvent(*system_event);
  } else {
    assert(false);
    return {};
  }
}

}  // namespace scada
