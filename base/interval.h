#pragma once

#include "base/check.h"

#include <algorithm>

template <class T>
using Interval = std::pair<T, T>;

template <class T>
inline bool IsValidInterval(const Interval<T>& interval) {
  return interval.first <= interval.second;
}

template <class T>
inline bool IsEmptyInterval(const Interval<T>& interval) {
  base::Check(IsValidInterval(interval));
  return interval.first == interval.second;
}

template <class T>
inline bool IntervalContains(const Interval<T>& a, const Interval<T>& b) {
  base::Check(IsValidInterval(a));
  base::Check(IsValidInterval(b));
  return a.first <= b.first && b.second <= a.second;
}

template <class T>
inline bool IntervalsOverlap(const Interval<T>& a, const Interval<T>& b) {
  base::Check(IsValidInterval(a));
  base::Check(IsValidInterval(b));
  return a.first < b.second && b.first < a.second;
}

template <class T>
inline Interval<T> IntervalIntersection(const Interval<T>& a,
                                        const Interval<T>& b) {
  base::Check(IntervalsOverlap(a, b));
  return {std::max(a.first, b.first), std::min(a.second, b.second)};
}

template <class T>
inline Interval<T> IntervalUnion(const Interval<T>& a, const Interval<T>& b) {
  base::Check(IsValidInterval(a));
  base::Check(IsValidInterval(b));
  return {std::min(a.first, b.first), std::max(a.second, b.second)};
}
