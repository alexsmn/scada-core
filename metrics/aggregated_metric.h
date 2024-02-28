#pragma once

#include <cassert>
#include <string>

template <class T>
class AggregatedMetric {
 public:
  std::size_t count() const { return count_; }
  bool empty() const { return count_ == 0; }

  T min() const {
    assert(!empty());
    return min_;
  }

  T max() const {
    assert(!empty());
    return max_;
  }

  T mean() const {
    assert(!empty());
    return sum_ / count_;
  }

  T sum() const {
    assert(!empty());
    return sum_;
  }

  AggregatedMetric& operator()(T value) {
    if (count_ == 0) {
      min_ = max_ = sum_ = value;
      count_ = 1;

    } else {
      if (value < min_)
        min_ = value;
      else if (value > max_)
        max_ = value;
      sum_ += value;
      ++count_;
    }

    return *this;
  }

  void reset() { count_ = 0; }

 private:
  std::size_t count_ = 0;
  T max_;
  T min_;
  T sum_;
};

template <class T>
struct AggregatedCounter {
  size_t counter = 0;
  AggregatedMetric<T> metric;

  void operator++() {
    ++counter;
    metric(counter);
  }

  void operator--() {
    --counter;
    metric(counter);
  }
};
