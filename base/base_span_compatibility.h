#pragma once

#include "base/containers/span.h"

#include <span>

template <class T>
inline std::span<T> AsStdSpan(base::span<T> span) {
  return {span.data(), span.size()};
}

template <class T>
inline base::span<T> AsBaseSpan(std::span<T> span) {
  return {span.data(), span.size()};
}
