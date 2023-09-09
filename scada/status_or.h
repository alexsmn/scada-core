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

  const Status& status() const& {
    assert(!ok());
    return std::get<Status>(value_);
  }

  Status status() && {
    assert(!ok());
    return std::get<Status>(std::move(value_));
  }

  T& operator*() {
    assert(ok());
    return std::get<T>(value_);
  }

  const T& operator*() const {
    assert(ok());
    return std::get<T>(value_);
  }

  T* operator->() {
    assert(ok());
    return &std::get<T>(value_);
  }

  const T* operator->() const {
    assert(ok());
    return &std::get<T>(value_);
  }

 private:
  std::variant<Status, T> value_;
};

template <class T>
class [[nodiscard]] StatusCodeOr {
 public:
  StatusCodeOr(StatusCode status_code) : value_{status_code} {
    assert(IsBad(this->status_code()));
  }

  StatusCodeOr(T value) : value_{std::move(value)} {}

  bool ok() const { return std::holds_alternative<T>(value_); }

  const StatusCode& status_code() const& {
    assert(!ok());
    return std::get<StatusCode>(value_);
  }

  StatusCode status_code() && {
    assert(!ok());
    return std::get<StatusCode>(std::move(value_));
  }

  T& operator*() {
    assert(ok());
    return std::get<T>(value_);
  }

  const T& operator*() const {
    assert(ok());
    return std::get<T>(value_);
  }

  T* operator->() {
    assert(ok());
    return &std::get<T>(value_);
  }

  const T* operator->() const {
    assert(ok());
    return &std::get<T>(value_);
  }

  template <class T, class F>
  inline auto map(F&& f) const& {
    // using R = std::invoke_result_t<F, T>;
    return ok() ? f(*this) : status_code();
  }

  template <class T, class F>
  inline auto map(F&& f) && {
    // using R = std::invoke_result_t<F, T>;
    return ok() ? f(*std::move(this)) : status_code();
  }

 private:
  std::variant<StatusCode, T> value_;
};

}  // namespace scada

template <class T>
inline std::ostream& operator<<(std::ostream& os,
                                const scada::StatusOr<T>& st) {
  return st.ok() ? (os << *st) : (os << st.status());
}

template <class T>
inline std::ostream& operator<<(std::ostream& os,
                                const scada::StatusCodeOr<T>& st) {
  return st.ok() ? (os << *st) : (os << st.status_code());
}
