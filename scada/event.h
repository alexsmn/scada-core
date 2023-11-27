#pragma once

#include "base/debug_util.h"
#include "base/struct_writer.h"
#include "scada/data_value.h"
#include "scada/standard_node_ids.h"

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

// Cannot be zero.
using EventId = scada::UInt64;

// Corresponds to the `BaseEventType`.
// https://reference.opcfoundation.org/Core/Part5/v105/docs/6.4.2
// TODO: Introduce an event ID and remove the ack ID.
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

  [[nodiscard]] bool is_valid() const;

  auto operator<=>(const Event&) const = default;

  NodeId event_type_id = scada::id::SystemEventType;
  // `event_id` is zero until it's processed by server. And never can become
  // zero after that.
  EventId event_id = 0;
  // `time` cannot be null.
  DateTime time;
  // `receive_time` is assigned by server. It's null until the event is
  // processed by server.
  DateTime receive_time;
  scada::UInt32 change_mask = 0;
  scada::UInt32 severity = kSeverityNormal;
  // `node_id` can be null. TODO: Describe when it's null.
  // TODO: Rename to `source_node_id`.
  NodeId node_id;
  // `user_id` can be null.
  NodeId user_id;
  // `value` can be null.
  Variant value;
  Qualifier qualifier;
  // TODO: Require non-empty message.
  scada::LocalizedText message;
  bool acked = false;
  // `acknowledged_time` must be non-null if `acked` is true.
  DateTime acknowledged_time;
  // `acknowledged_user_id` can be null if `acked` is true.
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

  auto operator<=>(const ModelChangeEvent&) const = default;

  NodeId node_id;

  // |type_definition_id| is only set for |NodeAdded| event.
  NodeId type_definition_id;

  uint8_t verb = 0;

  static const NumericId event_type_id = id::GeneralModelChangeEventType;
};

struct SemanticChangeEvent {
  auto operator<=>(const SemanticChangeEvent&) const = default;

  NodeId node_id;

  static const NumericId event_type_id = id::SemanticChangeEventType;
};

struct EventFilter {
  enum EventType { ACKED = 1 << 0, UNACKED = 1 << 1 };

  static const unsigned ALL_TYPES = 0;

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

  auto operator<=>(const EventFilter&) const = default;

  // A bit mask of `EventType`. Zero means no filter, for both real-time and
  // historical events.
  unsigned types = 0;

  std::vector<NodeId> of_type;
  std::vector<NodeId> child_of;
};

inline bool Event::is_valid() const {
  if (event_id == 0 || time.is_null() || receive_time.is_null()) {
    return false;
  }

  if (acked) {
    if (acknowledged_time.is_null()) {
      return false;
    }
  } else {
    if (!acknowledged_time.is_null() || !acknowledged_user_id.is_null()) {
      return false;
    }
  }

  return true;
}

inline std::ostream& operator<<(std::ostream& stream, const Event& event) {
  StructWriter{stream}
      .AddField("event_type_id", event.event_type_id)
      .AddField("event_id", event.event_id)
      .AddField("time", event.time)
      .AddField("receive_time", event.receive_time)
      .AddField("node_id", event.node_id)
      .AddField("user_id", event.user_id)
      .AddField("value", event.value)
      .AddField("message", event.message)
      .AddField("acked", event.acked)
      .AddField("acknowledged_user_id", event.acknowledged_user_id)
      .AddField("acknowledged_time", event.acknowledged_time);
  return stream;
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

inline std::ostream& operator<<(std::ostream& stream,
                                const EventFilter& event_filter) {
  constexpr std::string_view kTypeBitStrings[] = {"ACKED", "UNACKED"};
  StructWriter{stream}
      .AddBitMaskField("types", event_filter.types, kTypeBitStrings)
      .AddField("of_type", event_filter.of_type)
      .AddField("child_of", event_filter.child_of);
  return stream;
}

}  // namespace scada
