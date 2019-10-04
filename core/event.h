#pragma once

#include "base/strings/string16.h"
#include "core/configuration_types.h"
#include "core/data_value.h"
#include "core/standard_node_ids.h"

namespace scada {

// sys event severities
enum EventSeverity {
  kSeverityMin = 0,        // silent
  kSeverityVerbose = 20,   // verbose
  kSeverityNormal = 50,    // normal
  kSeverityWarning = 60,   // warning
  kSeverityCritical = 80,  // critical
  kSeverityMax = 100       // max
};

using EventAcknowledgeId = unsigned;

class Event {
 public:
  enum ChangeFlags {
    EVT_VAL = 0x0001,     // value changed
    EVT_QUAL = 0x0002,    // quality changed
    EVT_LIM = 0x0004,     // limit changed
    EVT_USER = 0x0008,    // user event
    EVT_SUBS = 0x0010,    // subsystem event
    EVT_MAN = 0x0020,     // manual input
    EVT_LOCK = 0x0040,    // locked
    EVT_CTRL = 0x0080,    // control
    EVT_BACKUP = 0x0100,  // locked
  };

  scada::NodeId event_type_id;
  base::Time time;
  unsigned change_mask = 0;
  unsigned severity = kSeverityNormal;
  NodeId node_id;
  NodeId user_id;
  Variant value;
  Qualifier qualifier;
  base::string16 message;
  bool acked = false;
  EventAcknowledgeId acknowledge_id;
  DateTime acknowledged_time;
  NodeId acknowledged_user_id;
};

struct ModelChangeEvent {
  enum Verb : uint8_t {
    NodeAdded = 1 << 0,
    NodeDeleted = 1 << 1,
    ReferenceAdded = 1 << 2,
    ReferenceDeleted = 1 << 3,
    DataTypeChanged = 1 << 4,
  };

  scada::NodeId node_id;
  scada::NodeId type_definition_id;
  uint8_t verb = 0;

  static const NumericId event_type_id = id::GeneralModelChangeEventType;
};

struct SemanticChangeEvent {
  scada::NodeId node_id;

  static const NumericId event_type_id = scada::id::SemanticChangeEventType;
};

inline bool operator==(const ModelChangeEvent& a, const ModelChangeEvent& b) {
  return a.node_id == b.node_id &&
         a.type_definition_id == b.type_definition_id && a.verb == b.verb;
}

struct EventFilter {
  enum EventType { ACKED = 1, UNACKED = 2 };
  unsigned types = 0;

  std::vector<scada::NodeId> of_type;
  std::vector<scada::NodeId> child_of;
};

}  // namespace scada

inline std::ostream& operator<<(std::ostream& stream,
                                const scada::ModelChangeEvent& e) {
  return stream << "{" << e.node_id << ", " << e.type_definition_id << ", "
                << static_cast<unsigned>(e.verb) << "}";
}
