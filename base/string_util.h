#pragma once

#include "base/containers/span.h"

#include <string_view>
#include <vector>

inline bool IsEqualNoCase(std::string_view a, std::string_view b) {
  if (a.size() != b.size())
    return false;
  if (a.empty())
    return true;
  return std::equal(a.begin(), a.end(), b.begin(),
                    [](char a, char b) { return tolower(a) == tolower(b); });
}

inline bool IsEqualNoCase(std::wstring_view a, std::wstring_view b) {
  if (a.size() != b.size())
    return false;
  if (a.empty())
    return true;
  return std::equal(a.begin(), a.end(), b.begin(), [](wchar_t a, wchar_t b) {
    return tolower(a) == tolower(b);
  });
}

inline std::vector<std::string_view> SplitString(std::string_view str,
                                                 std::string_view delimiter) {
  if (str.empty())
    return {};

  std::vector<std::string_view> parts;

  for (;;) {
    auto p = str.find(delimiter);
    if (p == delimiter.npos) {
      parts.push_back(str);
      break;
    }

    parts.push_back(str.substr(0, p));
    str = str.substr(p + delimiter.size());
  }

  return parts;
}

inline std::string JoinStrings(base::span<const std::string_view> strings,
                               std::string_view delimiter) {
  if (strings.empty())
    return {};

  std::string result{strings[0]};
  for (size_t i = 1; i < strings.size(); ++i) {
    result += delimiter;
    result += strings[i];
  }

  return result;
}
