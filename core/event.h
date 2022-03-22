#pragma once

#include "base/debug_util.h"
#include "base/strings/string16.h"
#include "base/struct_writer.h"
#include "core/data_value.h"
#include "core/standard_node_ids.h"

#include <string>

namespace scada {

// sys event severities
enum EventSeverity : unsigned {
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

  NodeId event_type_id;
  DateTime time;
  unsigned change_mask = 0;
  unsigned severity = kSeverityNormal;
  NodeId node_id;
  NodeId user_id;
  Variant value;
  Qualifier qualifier;
  scada::LocalizedText message;
  bool acked = false;
  EventAcknowledgeId acknowledge_id = 0;
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

  ModelChangeEvent& set_verb(uint8_t verb) {
    this->verb = verb;
    return *this;
  }

  NodeId node_id;

  // |type_definition_id| is only set for |NodeAdded| event.
  NodeId type_definition_id;

  uint8_t verb = 0;

  static const NumericId event_type_id = id::GeneralModelChangeEventType;
};

struct SemanticChangeEvent {
  NodeId node_id;

  static const NumericId event_type_id = id::SemanticChangeEventType;
};

struct EventFilter {
  enum EventType { ACKED = 1, UNACKED = 2 };

  EventFilter& set_of_type(std::vector<NodeId> of_type) {
    this->of_type = std::move(of_type);
    return *this;
  }

  EventFilter& add_of_type(NodeId type_definition_id) {
    of_type.emplace_back(std::move(type_definition_id));
    return *this;
  }

  EventFilter& set_child_of(std::vector<NodeId> child_of) {
    this->child_of = std::move(child_of);
    return *this;
  }

  EventFilter& add_child_of(NodeId parent_id) {
    child_of.emplace_back(std::move(parent_id));
    return *this;
  }

  unsigned types = 0;

  std::vector<NodeId> of_type;
  std::vector<NodeId> child_of;
};

inline bool operator==(const ModelChangeEvent& a, const ModelChangeEvent& b) {
  return a.node_id == b.node_id &&
         a.type_definition_id == b.type_definition_id && a.verb == b.verb;
}

inline bool operator==(const SemanticChangeEvent& a,
                       const SemanticChangeEvent& b) {
  return a.node_id == b.node_id;
}

inline std::ostream& operator<<(std::ostream& stream,
                                const ModelChangeEvent& e) {
  constexpr std::string_view kVerbBitStrings[] = {
      "NodeAdded",        "NodeDeleted",     "ReferenceAdded",
      "ReferenceDeleted", "DataTypeChanged",
  };
  StructWriter{stream}
      .AddField("node_id", e.node_id)
      .AddField("type_definition_id", e.type_definition_id)
      .AddBitMaskField("verb", e.verb, kVerbBitStrings);
  return stream;
}

inline std::ostream& operator<<(std::ostream& stream,
                                const SemanticChangeEvent& e) {
  StructWriter{stream}.AddField("node_id", e.node_id);
  return stream;
}

}  // namespace scada
