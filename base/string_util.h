#pragma once

#include "base/containers/span.h"

#include <numeric>
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

// |boost::split| is not useful for delimiter string.
// |base::SplitString| works only with base::StringView.
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

inline std::vector<std::string_view> SplitString(std::string_view str,
                                                 char delimiter) {
  return SplitString(str, std::string_view{&delimiter, 1});
}

// |boost::algorithm::join| does not support |std::string_view|.
inline std::string JoinStrings(base::span<const std::string_view> strings,
                               std::string_view delimiter) {
  if (strings.empty())
    return {};

  const auto strings_combined_size = std::accumulate(
      strings.begin(), strings.end(), 0,
      [](std::size_t sum, std::string_view str) { return sum + str.size(); });
  const auto delimiters_size = delimiter.size() * (strings.size() - 1);
  const auto result_size = strings_combined_size + delimiters_size;

  std::string result;
  result.reserve(result_size);

  result += strings[0];
  for (size_t i = 1; i < strings.size(); ++i) {
    result += delimiter;
    result += strings[i];
  }

  assert(result.capacity() == result_size);
  return result;
}
