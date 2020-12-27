#include "core/debug_util.h"

std::string BitMaskToString(unsigned bit_mask,
                            base::span<const std::string_view> bit_strings) {
  std::string result = "[";
  bool first = true;
  for (std::size_t i = 0; i < bit_strings.size(); ++i) {
    if (bit_mask & (1 << i)) {
      if (!first)
        result += ",";
      result += '"';
      result += bit_strings[i];
      result += '"';
      first = false;
    }
  }
  result += "]";
  return result;
}
