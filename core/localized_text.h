#pragma once

#include "base/strings/string_piece.h"

#include <string>

#define LOCALIZED_TEXT(text) L##text

namespace scada {

using LocalizedText = std::wstring;

LocalizedText ToLocalizedText(base::StringPiece string);

inline const LocalizedText& ToLocalizedText(const std::wstring& string) {
  return string;
}

inline LocalizedText ToLocalizedText(std::wstring&& string) {
  return string;
}

}  // namespace scada

std::string ToString(const scada::LocalizedText& text);

inline const std::wstring& ToString16(
    const scada::LocalizedText& localized_text) {
  return localized_text;
}
