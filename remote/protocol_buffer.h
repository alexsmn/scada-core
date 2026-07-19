#pragma once

#include "base/check.h"

#include <cstdint>
#include <string>

namespace protocol {

typedef uint32_t MessageSizeType;

constexpr size_t kHeaderSize = sizeof(MessageSizeType);

// Read-buffer size for the framed Scada/Remote protocol. Messages are not
// chunked (unlike OPC UA), so a peer's read buffer must be at least as large as
// the biggest single message it will receive — otherwise the transport aborts
// with "Incoming message exceeds read buffer" and the session disconnects. The
// former 1 MiB buffer was too small for a full object-tree browse / attribute
// response against a real config (the client E2E ExpandsObjectTreeLabels lost
// the connection mid-load and only the root node rendered). 16 MiB comfortably
// covers realistic responses while remaining a bounded cap against a corrupt
// 32-bit size header requesting a multi-GB allocation.
constexpr size_t kMaxMessageSize = 16 * 1024 * 1024;

inline size_t GetMessagePayloadSize(std::span<const char> message) {
  base::Check(message.size() >= sizeof(MessageSizeType));
  return reinterpret_cast<const MessageSizeType&>(message[0]);
}

inline const void* GetMessagePayload(std::span<const char> message) {
  base::Check(message.size() >= sizeof(MessageSizeType));
  return &message[sizeof(MessageSizeType)];
}

inline size_t GetIncomingMessageSize(std::span<const char> message) {
  if (message.size() < sizeof(MessageSizeType)) {
    return sizeof(MessageSizeType);
  }
  return sizeof(MessageSizeType) + GetMessagePayloadSize(message);
}

inline void AppendMessage(std::string& message, const void* data, size_t size) {
  const char* bytes = reinterpret_cast<const char*>(data);
  message.insert(message.end(), bytes, bytes + size);
}

inline void PrependMessageSize(std::string& message) {
  base::Check(message.empty());
  message.resize(sizeof(MessageSizeType));
}

inline void UpdateMessageSize(std::string& message) {
  reinterpret_cast<MessageSizeType&>(message[0]) =
      message.size() - sizeof(MessageSizeType);
}

}  // namespace protocol