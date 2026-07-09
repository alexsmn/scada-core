#pragma once

#include "base/lifetime.h"
#include "scada/node_id.h"

#include <vector>

namespace scada {

struct EventFilter {
  enum EventType { ACKED = 1 << 0, UNACKED = 1 << 1 };

  static const unsigned ALL_TYPES = 0;

  EventFilter& set_of_type(std::vector<NodeId> of_type) SCADA_LIFETIME_BOUND {
    this->of_type = std::move(of_type);
    return *this;
  }

  EventFilter& add_of_type(const NodeId& type_definition_id)
      SCADA_LIFETIME_BOUND {
    of_type.emplace_back(type_definition_id);
    return *this;
  }

  EventFilter& set_child_of(std::vector<NodeId> child_of) SCADA_LIFETIME_BOUND {
    this->child_of = std::move(child_of);
    return *this;
  }

  EventFilter& add_child_of(const NodeId& parent_id) SCADA_LIFETIME_BOUND {
    child_of.emplace_back(parent_id);
    return *this;
  }

  bool operator==(const EventFilter&) const = default;

  // A bit mask of `EventType`. Zero means no filter, for both real-time and
  // historical events.
  unsigned types = 0;

  std::vector<NodeId> of_type;
  std::vector<NodeId> child_of;
};

std::ostream& operator<<(std::ostream& stream, const EventFilter& event_filter);

}  // namespace scada