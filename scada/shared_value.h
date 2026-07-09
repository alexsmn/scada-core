#pragma once

#include "base/lifetime.h"

#include <memory>

// Move operator is also defined, to enable `NodeId` move.
template <class T>
class SharedValue {
 public:
  template <class U>
  explicit SharedValue(U&& value)
      : value_{std::make_shared<T>(std::forward<U>(value))} {}

  const T& get() const SCADA_LIFETIME_BOUND { return *value_; }

  auto operator==(const SharedValue& other) const {
    return *value_ == *other.value_;
  }

  auto operator<=>(const SharedValue& other) const {
    return *value_ <=> *other.value_;
  }

 private:
  std::shared_ptr<const T> value_;
};
