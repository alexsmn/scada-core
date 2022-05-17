#pragma once

#include <ostream>
#include <string>
#include <string_view>

#define LOCALIZED_TEXT(text) L##text

namespace scada {

using LocalizedText = std::u16string;

LocalizedText ToLocalizedText(std::string_view string);

inline LocalizedText ToLocalizedText(const std::u16string_view& string) {
  return LocalizedText{string.data(), string.size()};
}

inline const LocalizedText& ToLocalizedText(const std::u16string& string) {
  return string;
}

inline LocalizedText ToLocalizedText(std::u16string&& string) {
  return string;
}

}  // namespace scada

std::string ToString(const scada::LocalizedText& text);

inline const std::u16string& ToString16(
    const scada::LocalizedText& localized_text) {
  return localized_text;
}
