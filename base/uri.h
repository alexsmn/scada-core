#pragma once

#include <string>

// Escapes characters in text suitable for use as a query parameter value.
// We %XX everything except alphanumerics and -_.!~*'()
// Spaces change to "+" unless you pass usePlus=false.
// This is basically the same as encodeURIComponent in javascript.
std::string EscapeQueryParamValue(std::string_view text, bool use_plus);

// Escapes a partial or complete file/pathname.  This includes:
// non-printable, non-7bit, and (including space)  "#%:<>?[\]^`{|}
std::string EscapePath(std::string_view path);

namespace base {

namespace UnescapeRule {
enum Type : unsigned {
  NORMAL = 0,
  SPACES = 1 << 0,
  PATH_SEPARATORS = 1 << 1,
};
}  // namespace UnescapeRule

// Unescapes |escaped_text| and returns the result.
// Unescapes %XX sequences. If a sequence is not a valid escape, it is left
// unchanged. Rules control which characters are unescaped beyond basic %XX.
std::string UnescapeURLComponent(std::string_view escaped_text,
                                 unsigned rules);

}  // namespace base
