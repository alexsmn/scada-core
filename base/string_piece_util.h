#pragma once

#include "base/strings/string_piece.h"

#include <string_view>

inline constexpr base::StringPiece ToStringPiece(
    std::string_view str) noexcept {
  return {str.data(), str.size()};
}

inline constexpr base::StringPiece16 ToStringPiece(
    std::wstring_view str) noexcept {
  return {str.data(), str.size()};
}
