#pragma once

#include "base/lifetime.h"
#include "scada/node_id.h"

#include <vector>

namespace scada {

struct EventFilter {
  // RETAINED widens UNACKED: it also selects the latest notification of every
  // Condition still active although acknowledged — Part 9's Retain (OPC UA
  // Part 9 §5.5.2 ConditionType,
  // https://reference.opcfoundation.org/Core/Part9/v105/docs/5.5.2; see
  // `IsRetained` in scada/event.h). Send it together with UNACKED: a server
  // built before backlog 1117 then still answers the unacknowledged set rather
  // than every event it holds.
  enum EventType { ACKED = 1 << 0, UNACKED = 1 << 1, RETAINED = 1 << 2 };

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

  // Which event fields a live subscription asks the server for.
  enum class Fields {
    // Nothing named: our own server projects its full SCADA field list, which
    // carries every scada::Event member.
    kScada,
    // The BaseEventType fields any OPC UA server can project (OPC UA Part 5
    // §6.4.2 BaseEventType,
    // https://reference.opcfoundation.org/Core/Part5/v105/docs/6.4.2).
    kStandard,
    // kStandard plus a Condition's ConditionId and AckedState/Id (OPC UA
    // Part 9 §5.5.2 ConditionType and §5.7.2 AcknowledgeableConditionType,
    // https://reference.opcfoundation.org/Core/Part9/v105/docs/5.5.2).
    kStandardWithConditions,
    // kScada plus our own server's Condition fields, ConditionId and
    // ActiveState/Id (OPC UA Part 9 §5.8.2 AlarmConditionType,
    // https://reference.opcfoundation.org/Core/Part9/v105/docs/5.8.2), named
    // explicitly so a client built before them still gets kScada's list
    // (backlog 1117).
    kScadaWithConditions,
  };
  // A third-party server knows nothing of the SCADA extension fields, and a
  // strict one rejects a filter that names them (backlog 1055).
  Fields fields = Fields::kScada;
};

std::ostream& operator<<(std::ostream& stream, const EventFilter& event_filter);

}  // namespace scada