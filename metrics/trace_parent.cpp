#include "metrics/trace_parent.h"

#include <algorithm>

namespace {

// `00-` + 32 hex + `-` + 16 hex + `-` + 2 hex.
constexpr size_t kTraceParentLength = 2 + 1 + 32 + 1 + 16 + 1 + 2;

constexpr char kHexDigits[] = "0123456789abcdef";

// Returns the value of a lowercase hex digit, or -1. The spec requires
// lowercase hex (https://www.w3.org/TR/trace-context/#traceparent-header);
// uppercase input is rejected rather than normalized.
int HexValue(char c) {
  if (c >= '0' && c <= '9')
    return c - '0';
  if (c >= 'a' && c <= 'f')
    return c - 'a' + 10;
  return -1;
}

// Decodes `text` (exactly 2*N lowercase hex chars) into `bytes`.
template <size_t N>
bool DecodeHex(std::string_view text, std::array<uint8_t, N>& bytes) {
  for (size_t i = 0; i < N; ++i) {
    int high = HexValue(text[2 * i]);
    int low = HexValue(text[2 * i + 1]);
    if (high < 0 || low < 0)
      return false;
    bytes[i] = static_cast<uint8_t>((high << 4) | low);
  }
  return true;
}

template <size_t N>
void EncodeHex(const std::array<uint8_t, N>& bytes, std::string& out) {
  for (uint8_t byte : bytes) {
    out += kHexDigits[byte >> 4];
    out += kHexDigits[byte & 0x0f];
  }
}

template <size_t N>
bool AllZero(const std::array<uint8_t, N>& bytes) {
  return std::ranges::all_of(bytes, [](uint8_t b) { return b == 0; });
}

}  // namespace

std::optional<W3CTraceParent> ParseTraceParent(std::string_view value) {
  if (value.size() != kTraceParentLength)
    return std::nullopt;
  if (value[0] != '0' || value[1] != '0')  // Only version 00 is supported.
    return std::nullopt;
  if (value[2] != '-' || value[35] != '-' || value[52] != '-')
    return std::nullopt;

  W3CTraceParent result;
  if (!DecodeHex(value.substr(3, 32), result.trace_id))
    return std::nullopt;
  if (!DecodeHex(value.substr(36, 16), result.span_id))
    return std::nullopt;

  std::array<uint8_t, 1> flags = {};
  if (!DecodeHex(value.substr(53, 2), flags))
    return std::nullopt;
  result.flags = flags[0];

  // All-zero trace/span ids are invalid per the spec.
  if (AllZero(result.trace_id) || AllZero(result.span_id))
    return std::nullopt;

  return result;
}

std::string FormatTraceParent(const W3CTraceParent& trace_parent) {
  std::string result;
  result.reserve(kTraceParentLength);
  result += "00-";
  EncodeHex(trace_parent.trace_id, result);
  result += '-';
  EncodeHex(trace_parent.span_id, result);
  result += '-';
  EncodeHex(std::array<uint8_t, 1>{trace_parent.flags}, result);
  return result;
}

bool IsTraceParent(std::string_view value) {
  return ParseTraceParent(value).has_value();
}
