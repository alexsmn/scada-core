#pragma once

#include <functional>

// Stores a value. When the value changes, calls a handler.
//
// WARNING: Triggers change handler on destruction.
template <class T>
class Cell {
 public:
  using ChangeHandler = std::function<void(const T& value)>;

  explicit Cell(ChangeHandler change_handler);
  ~Cell();

  Cell(const Cell&) = delete;
  Cell& operator=(const Cell&) = delete;

  const T& get() const { return value_; }
  void Set(const T& value);

 private:
  const ChangeHandler change_handler_;

  T value_;
};

template <class T>
inline Cell<T>::Cell(ChangeHandler change_handler)
    : change_handler_{std::move(change_handler)} {}

template <class T>
inline Cell<T>::~Cell() {
  // TODO: Remove the change handler triggering on destruction.
  // Avoid unset value.
  if (value_) {
    change_handler_(true);
  }
}

template <class T>
inline void Cell<T>::Set(const T& value) {
  if (value_ == value) {
    return;
  }

  value_ = value;
  change_handler_(value);
}
