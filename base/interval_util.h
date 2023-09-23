#pragma once

#include "base/interval.h"

#include <vector>

// Cannot use `std::span` since `std::vector<Interval<T>>` doesn't case to
// `std::span<const T>` well.

template <class T>
inline bool AreValidIntervals(const std::vector<Interval<T>>& intervals) {
  return std::ranges::all_of(intervals, &IsValidInterval<T>) &&
         std::ranges::is_sorted(intervals);
}

template <class T>
inline bool IntervalsContain(const std::vector<Interval<T>>& intervals,
                             const Interval<T>& interval) {
  assert(IsValidInterval(interval));
  assert(AreValidIntervals(intervals));

  auto i = std::ranges::lower_bound(
      intervals, interval.first, std::less{},
      [](const auto& interval) { return interval.second; });

  if (i == intervals.end()) {
    return false;
  }

  auto& found_interval = *i;
  return IntervalContains(found_interval, interval);
}

template <class T>
inline bool UnionIntervals(std::vector<Interval<T>>& intervals,
                           const Interval<T>& interval) {
  assert(IsValidInterval(interval));
  assert(AreValidIntervals(intervals));

  // Find the position to remove from. It's the last interval that ends just
  // before the new interval start.

  auto i = std::lower_bound(intervals.begin(), intervals.end(), interval.first,
                            [](const auto& interval, const auto& bound) {
                              return interval.second < bound;
                            });

  // If the new interval is in the end or it doesn't intersect anything, then
  // just insert the new interval.
  if (i == intervals.end() || interval.second < i->first) {
    intervals.insert(i, interval);
    return true;
  }

  // If the interval is fully contained, then ignore it.
  if (i->first <= interval.first && interval.second <= i->second)
    return false;

  // Find the position to remove until. It's the interval that starts after or
  // exactly on the new interval end. The actual position is after this
  // interval.

  auto j = std::upper_bound(i, intervals.end(), interval.second,
                            [](const auto& bound, const auto& interval) {
                              return bound < interval.first;
                            });

  // Update the first interval and remove others.

  i->first = std::min(i->first, interval.first);
  i->second = std::max(std::prev(j)->second, interval.second);

  assert(i != j);
  intervals.erase(std::next(i), j);

  assert(std::is_sorted(intervals.begin(), intervals.end()));
  return true;
}
