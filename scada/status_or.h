#pragma once

#include "scada/status.h"

#include <cassert>
#include <ostream>
#include <variant>

namespace scada {

template <class T>
class [[nodiscard]] StatusOr {
 public:
  StatusOr(Status status) : value_{std::move(status)} {
    assert(!this->status());
  }

  StatusOr(StatusCode status_code) : StatusOr{Status(status_code)} {}

  StatusOr(T value) : value_{std::move(value)} {}

  bool ok() const { return std::holds_alternative<T>(value_); }

  Status status() const {
    const auto* status = std::get_if<Status>(&value_);
    return status ? *status : Status{StatusCode::Good};
  }

  T& value() {
    assert(ok());
    return std::get<T>(value_);
  }

  const T& value() const {
    assert(ok());
    return std::get<T>(value_);
  }

  T& operator*() { return value(); }

  const T& operator*() const { return value(); }

  T* operator->() {
    assert(ok());
    return &std::get<T>(value_);
  }

  const T* operator->() const {
    assert(ok());
    return &std::get<T>(value_);
  }

 private:
  using StatusVariant = std::variant<Status, T>;

  StatusVariant value_;
};

}  // namespace scada

template <class T>
inline std::ostream& operator<<(std::ostream& os,
                                const scada::StatusOr<T>& st) {
  return st.ok() ? (os << *st) : (os << st.status());
}
