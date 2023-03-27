#pragma once

#include <base/strings/string_piece.h>
#include <string_view>

template <class CharT>
inline constexpr base::BasicStringPiece<CharT> AsStringPiece(
    std::basic_string_view<CharT> str) noexcept {
  return base::BasicStringPiece<CharT>{str.data(), str.size()};
}

template <class CharT>
inline constexpr std::basic_string_view<CharT> AsStringView(
    base::BasicStringPiece<CharT> str) noexcept {
  return std::basic_string_view<CharT>{str.data(), str.size()};
}
