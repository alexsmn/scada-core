#pragma once

#include "scada/event.h"

#include <optional>

namespace scada {

// Encodes an EventId as the ByteString mandated for the BaseEventType EventId
// field (OPC UA Part 5 §6.4.2,
// https://reference.opcfoundation.org/Core/Part5/v105/docs/6.4.2). Layout is
// 8 bytes, big-endian, so lexicographic ByteString order equals numeric order.
// The layout is frozen: EventIds cross tier boundaries and are resolved back
// by the Acknowledge method. Mirrors opcua::EncodeEventIdByteString.
inline ByteString EncodeEventIdByteString(EventId event_id) {
  ByteString bytes(8);
  for (int i = 0; i < 8; ++i) {
    bytes[i] = static_cast<char>((event_id >> (8 * (7 - i))) & 0xff);
  }
  return bytes;
}

// Decodes an EventId ByteString produced by EncodeEventIdByteString. Returns
// nullopt when the payload is not exactly 8 bytes (foreign or corrupt id).
inline std::optional<EventId> DecodeEventIdByteString(const ByteString& bytes) {
  if (bytes.size() != 8) {
    return std::nullopt;
  }
  EventId event_id = 0;
  for (int i = 0; i < 8; ++i) {
    event_id = (event_id << 8) | static_cast<unsigned char>(bytes[i]);
  }
  return event_id;
}

}  // namespace scada
