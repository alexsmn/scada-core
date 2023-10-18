#include "base/debug_util.h"

#include <boost/locale/encoding_utf.hpp>

std::ostream& operator<<(std::ostream& stream, const std::wstring& v) {
  return stream << boost::locale::conv::utf_to_utf<char>(v);
}

std::string BitMaskToString(unsigned bit_mask,
                            std::span<const std::string_view> bit_strings) {
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
