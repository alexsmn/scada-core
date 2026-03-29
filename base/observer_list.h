#pragma once

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <vector>

namespace base {

template <typename T>
class ObserverList {
 public:
  using iterator = typename std::vector<T*>::iterator;
  using const_iterator = typename std::vector<T*>::const_iterator;

  ObserverList() = default;

  void AddObserver(T* observer) {
    assert(observer);
    assert(!HasObserver(observer));
    observers_.push_back(observer);
  }

  void RemoveObserver(T* observer) {
    auto it = std::find(observers_.begin(), observers_.end(), observer);
    if (it != observers_.end()) {
      if (iteration_depth_ > 0) {
        // Mark as null; compact later.
        *it = nullptr;
        ++null_count_;
      } else {
        observers_.erase(it);
      }
    }
  }

  bool HasObserver(const T* observer) const {
    return std::find(observers_.begin(), observers_.end(), observer) !=
           observers_.end();
  }

  bool might_have_observers() const { return !observers_.empty(); }

  // Iteration support. Handles observer removal during iteration.
  class Iterator {
   public:
    explicit Iterator(ObserverList& list) : list_(list), index_(0) {
      ++list_.iteration_depth_;
    }
    ~Iterator() {
      if (--list_.iteration_depth_ == 0 && list_.null_count_ > 0) {
        list_.Compact();
      }
    }

    T* GetNext() {
      while (index_ < list_.observers_.size()) {
        T* observer = list_.observers_[index_++];
        if (observer)
          return observer;
      }
      return nullptr;
    }

   private:
    ObserverList& list_;
    size_t index_;
  };

  // Range-based for loop support.
  class RangeIterator {
   public:
    RangeIterator(ObserverList* list, size_t index)
        : list_(list), index_(index) {
      SkipNulls();
    }

    T& operator*() const { return *list_->observers_[index_]; }
    T* operator->() const { return list_->observers_[index_]; }

    RangeIterator& operator++() {
      ++index_;
      SkipNulls();
      return *this;
    }

    RangeIterator operator++(int) {
      auto tmp = *this;
      ++(*this);
      return tmp;
    }

    bool operator!=(const RangeIterator& other) const {
      return index_ != other.index_;
    }

   private:
    void SkipNulls() {
      while (index_ < list_->observers_.size() &&
             list_->observers_[index_] == nullptr)
        ++index_;
    }

    ObserverList* list_;
    size_t index_;
  };

  RangeIterator begin() {
    ++iteration_depth_;
    return RangeIterator{this, 0};
  }

  RangeIterator end() { return RangeIterator{this, observers_.size()}; }

  // Const iteration.
  class ConstRangeIterator {
   public:
    ConstRangeIterator(const ObserverList* list, size_t index)
        : list_(list), index_(index) {
      SkipNulls();
    }

    const T& operator*() const { return *list_->observers_[index_]; }
    const T* operator->() const { return list_->observers_[index_]; }

    ConstRangeIterator& operator++() {
      ++index_;
      SkipNulls();
      return *this;
    }

    ConstRangeIterator operator++(int) {
      auto tmp = *this;
      ++(*this);
      return tmp;
    }

    bool operator!=(const ConstRangeIterator& other) const {
      return index_ != other.index_;
    }

   private:
    void SkipNulls() {
      while (index_ < list_->observers_.size() &&
             list_->observers_[index_] == nullptr)
        ++index_;
    }

    const ObserverList* list_;
    size_t index_;
  };

  ConstRangeIterator begin() const {
    ++iteration_depth_;
    return ConstRangeIterator{this, 0};
  }

  ConstRangeIterator end() const {
    return ConstRangeIterator{this, observers_.size()};
  }

 private:
  void Compact() {
    observers_.erase(
        std::remove(observers_.begin(), observers_.end(), nullptr),
        observers_.end());
    null_count_ = 0;
  }

  std::vector<T*> observers_;
  mutable int32_t iteration_depth_ = 0;
  int32_t null_count_ = 0;
};

}  // namespace base
