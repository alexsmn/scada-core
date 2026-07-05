#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

// W3C Trace Context "traceparent" utilities
// (https://www.w3.org/TR/trace-context/#traceparent-header). The traceparent
// string is the cross-process trace-propagation carrier: it travels in the
// gRPC protocol `trace_id` request field and in the OPC UA
// RequestHeader.AdditionalHeader, and `scada::ServiceContext::trace_id()`
// holds it in-process while a request flows between tiers.

// Parsed form of a version-00 traceparent
// (`00-<32 hex trace id>-<16 hex span id>-<2 hex flags>`).
struct W3CTraceParent {
  std::array<uint8_t, 16> trace_id = {};
  std::array<uint8_t, 8> span_id = {};
  uint8_t flags = 0;

  // The sampled bit (https://www.w3.org/TR/trace-context/#sampled-flag).
  bool sampled() const { return (flags & 0x01) != 0; }
};

// Parses a version-00 traceparent string. Returns nullopt for anything
// malformed: wrong length or separators, non-lowercase-hex digits,
// unsupported version, or the all-zero (invalid per spec) trace or span id.
std::optional<W3CTraceParent> ParseTraceParent(std::string_view value);

// Formats the canonical lowercase-hex version-00 traceparent string.
std::string FormatTraceParent(const W3CTraceParent& trace_parent);

// True when `value` parses as a valid traceparent. Cheap validity check used
// before injecting a context's trace id into a wire request.
bool IsTraceParent(std::string_view value);
