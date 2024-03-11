#pragma once

#include <base/strings/string_piece.h>
#include <string_view>

template <class CharT>
inline constexpr std::basic_string_view<CharT> AsStringView(
    base::BasicStringPiece<CharT> str) noexcept {
  return std::basic_string_view<CharT>{str.data(), str.size()};
}
