#pragma once

#include "base/containers/span.h"

#include <vector>

template <class T>
inline std::vector<T> MakeVector(base::span<const T> span) {
  return std::vector<T>(span.begin(), span.end());
}
