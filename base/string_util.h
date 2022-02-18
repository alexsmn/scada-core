#pragma once

#include "base/containers/span.h"

#include <string_view>
#include <vector>

bool IsEqualNoCase(std::string_view a, std::string_view b);
bool IsEqualNoCase(std::u16string_view a, std::u16string_view b);

// |boost::split| is not useful for delimiter string.
// |base::SplitString| works only with base::StringView.
std::vector<std::string_view> SplitString(std::string_view str,
                                          std::string_view delimiter);

inline std::vector<std::string_view> SplitString(std::string_view str,
                                                 char delimiter) {
  return SplitString(str, std::string_view{&delimiter, 1});
}

// |boost::algorithm::join| does not support |std::string_view|.
std::string JoinStrings(base::span<const std::string_view> strings,
                        std::string_view delimiter);
