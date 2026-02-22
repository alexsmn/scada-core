#pragma once

#include <boost/range/iterator_range.hpp>
#include <span>

// Wraps std::span in boost::iterator_range for compatibility with
// Boost.Range adaptors (std::span does not satisfy Boost.Range concepts
// on MSVC).
template <class T, std::size_t Extent = std::dynamic_extent>
auto AsRange(std::span<T, Extent> s) {
  return boost::make_iterator_range(s.data(), s.data() + s.size());
}
