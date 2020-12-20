#pragma once

#include <vector>

template <class T, class Range>
inline std::vector<T> MakeVector(const Range& range) {
  return std::vector<T>(std::begin(range), std::end(range));
}
