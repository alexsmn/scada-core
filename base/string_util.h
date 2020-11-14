#pragma once

#include <string_view>

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
  return std::equal(a.begin(), a.end(), b.begin(),
                    [](char a, char b) { return tolower(a) == tolower(b); });
}
