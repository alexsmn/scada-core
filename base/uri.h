#pragma once

#include "base/strings/string_piece.h"

#include <string>

// Escapes characters in text suitable for use as a query parameter value.
// We %XX everything except alphanumerics and -_.!~*'()
// Spaces change to "+" unless you pass usePlus=false.
// This is basically the same as encodeURIComponent in javascript.
std::string EscapeQueryParamValue(base::StringPiece text, bool use_plus);

// Escapes a partial or complete file/pathname.  This includes:
// non-printable, non-7bit, and (including space)  "#%:<>?[\]^`{|}
std::string EscapePath(base::StringPiece path);
