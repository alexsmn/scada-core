#include "base/uri.h"

#include <cctype>

const char kHexString[] = "0123456789ABCDEF";

inline char IntToHex(int i) {
  // DCHECK_GE(i, 0) << i << " not a hex value";
  // DCHECK_LE(i, 15) << i << " not a hex value";
  return kHexString[i];
}

// A fast bit-vector map for ascii characters.
//
// Internally stores 256 bits in an array of 8 ints.
// Does quick bit-flicking to lookup needed characters.
struct Charmap {
  bool Contains(unsigned char c) const {
    return ((map[c >> 5] & (1 << (c & 31))) != 0);
  }

  uint32_t map[8];
};

// Given text to escape and a Charmap defining which values to escape,
// return an escaped string.  If use_plus is true, spaces are converted
// to +, otherwise, if spaces are in the charmap, they are converted to
// %20. And if keep_escaped is true, %XX will be kept as it is, otherwise, if
// '%' is in the charmap, it is converted to %25.
std::string Escape(std::string_view text,
                   const Charmap& charmap,
                   bool use_plus,
                   bool keep_escaped = false) {
  std::string escaped;
  escaped.reserve(text.length() * 3);
  for (unsigned int i = 0; i < text.length(); ++i) {
    unsigned char c = static_cast<unsigned char>(text[i]);
    if (use_plus && ' ' == c) {
      escaped.push_back('+');
    } else if (keep_escaped && '%' == c && i + 2 < text.length() &&
               std::isxdigit(text[i + 1]) && std::isxdigit(text[i + 2])) {
      escaped.push_back('%');
    } else if (charmap.Contains(c)) {
      escaped.push_back('%');
      escaped.push_back(IntToHex(c >> 4));
      escaped.push_back(IntToHex(c & 0xf));
    } else {
      escaped.push_back(c);
    }
  }
  return escaped;
}

// Everything except alphanumerics and !'()*-._~
// See RFC 2396 for the list of reserved characters.
static const Charmap kQueryCharmap = {{0xffffffffL, 0xfc00987dL, 0x78000001L,
                                       0xb8000001L, 0xffffffffL, 0xffffffffL,
                                       0xffffffffL, 0xffffffffL}};

// non-printable, non-7bit, and (including space)  "#%:<>?[\]^`{|}
static const Charmap kPathCharmap = {{0xffffffffL, 0xd400002dL, 0x78000000L,
                                      0xb8000001L, 0xffffffffL, 0xffffffffL,
                                      0xffffffffL, 0xffffffffL}};

std::string EscapeQueryParamValue(std::string_view text, bool use_plus) {
  return Escape(text, kQueryCharmap, use_plus);
}

std::string EscapePath(std::string_view path) {
  return Escape(path, kPathCharmap, false);
}

namespace {

int HexToInt(char c) {
  if (c >= '0' && c <= '9')
    return c - '0';
  if (c >= 'A' && c <= 'F')
    return c - 'A' + 10;
  if (c >= 'a' && c <= 'f')
    return c - 'a' + 10;
  return -1;
}

}  // namespace

namespace base {

std::string UnescapeURLComponent(std::string_view escaped_text,
                                 unsigned rules) {
  std::string result;
  result.reserve(escaped_text.size());
  for (size_t i = 0; i < escaped_text.size(); ++i) {
    if (escaped_text[i] == '%' && i + 2 < escaped_text.size()) {
      int high = HexToInt(escaped_text[i + 1]);
      int low = HexToInt(escaped_text[i + 2]);
      if (high >= 0 && low >= 0) {
        char c = static_cast<char>((high << 4) | low);
        bool should_unescape = true;
        if (c == ' ' && !(rules & UnescapeRule::SPACES))
          should_unescape = false;
        if ((c == '/' || c == '\\') && !(rules & UnescapeRule::PATH_SEPARATORS))
          should_unescape = false;
        if (should_unescape) {
          result.push_back(c);
          i += 2;
          continue;
        }
      }
    }
    result.push_back(escaped_text[i]);
  }
  return result;
}

}  // namespace base
