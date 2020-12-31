#pragma once

template <class Pred, class Iterator>
class FilterIterator {
 public:
  using iterator_category = typename Iterator::iterator_category;
  using value_type = typename Iterator::value_type;
  using difference_type = typename Iterator::difference_type;
  using pointer = typename Iterator::pointer;
  using reference = typename Iterator::reference;

  FilterIterator(Pred pred, Iterator it, Iterator end)
      : pred_(pred), it_(it), end_(end) {
    while (it_ != end_ && !pred_(*it_))
      ++it_;
  }

  FilterIterator& operator++() {
    ++it_;
    while (it_ != end_ && !pred_(*it_))
      ++it_;
    return *this;
  }

  FilterIterator& operator--() {
    --it_;
    while (!pred_(*it_))
      --it_;
    return *this;
  }

  bool operator==(const FilterIterator& other) const {
    return it_ == other.it_;
  }
  bool operator!=(const FilterIterator& other) const {
    return !operator==(other);
  }
  bool operator<(const FilterIterator& other) const { return it_ < other.it_; }

  value_type operator*() const { return *it_; }

 private:
  Pred pred_;
  Iterator it_;
  Iterator end_;
};

template <class Pred, class Iterator>
FilterIterator<Pred, Iterator> MakeFilterIterator(Pred pred,
                                                  Iterator it,
                                                  Iterator end) {
  return FilterIterator<Pred, Iterator>(pred, it, end);
}

template <class Func, class Iterator>
class TransformIterator {
 public:
  using iterator_category = typename Iterator::iterator_category;
  using value_type =
      typename std::invoke_result_t<Func, typename Iterator::value_type>;
  using difference_type = typename Iterator::difference_type;
  using pointer = value_type*;
  using reference = value_type&;

  TransformIterator(Func func, Iterator it) : func_(func), it_(it) {}

  TransformIterator& operator++() {
    ++it_;
    return *this;
  }
  TransformIterator& operator--() {
    --it_;
    return *this;
  }

  bool operator==(const TransformIterator& other) const {
    return it_ == other.it_;
  }
  bool operator!=(const TransformIterator& other) const {
    return !operator==(other);
  }
  bool operator<(const TransformIterator& other) const {
    return it_ < other.it_;
  }

  value_type operator*() const { return func_(*it_); }

 private:
  Func func_;
  Iterator it_;
};

template <class Func, class Iterator>
TransformIterator<Func, Iterator> MakeTransformIterator(Func func,
                                                        Iterator iterator) {
  return TransformIterator<Func, Iterator>(func, iterator);
}
