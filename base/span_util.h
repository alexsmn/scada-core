#pragma once

#include "base/containers/span.h"

#include <span>

template <class T>
inline constexpr base::span<T> AsBaseSpan(std::span<T> span) noexcept {
  return base::span<T>(span.data(), span.size());
}

template <class T>
inline constexpr base::span<T> AsStdSpan(base::span<T> span) noexcept {
  return std::span<T>(span.data(), span.size());
}
