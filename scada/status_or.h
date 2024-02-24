#pragma once

#include "scada/status.h"

#include <cassert>
#include <ostream>
#include <variant>

namespace scada {

template <class T>
class [[nodiscard]] StatusCodeOr {
 public:
  StatusCodeOr(StatusCode status_code) : value_{status_code} {
    assert(IsBad(this->status_code()));
  }

  StatusCodeOr(T value) : value_{std::move(value)} {}

  bool ok() const { return std::holds_alternative<T>(value_); }

  const T& value() const {
    assert(ok());
    return std::get<T>(value_);
  }

  StatusCode status_code() const {
    const auto* status_code = std::get_if<StatusCode>(&value_);
    return status_code ? *status_code : StatusCode::Good;
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

  template <class F>
  inline auto map(F&& f) const& {
    return ok() ? f(*this) : status_code();
  }

  template <class F>
  inline auto map(F&& f) && {
    return ok() ? f(*std::move(this)) : status_code();
  }

  inline T value_or(const T& other) const {
    const T* value = std::get_if<T>(&value_);
    return value ? *value : other;
  }

 private:
  std::variant<StatusCode, T> value_;
};

template <class T>
class [[nodiscard]] StatusOr {
 public:
  StatusOr(Status status) : value_{std::move(status)} {
    assert(!this->status());
  }

  StatusOr(StatusCode status_code) : StatusOr{Status(status_code)} {}

  StatusOr(T value) : value_{std::move(value)} {}

  StatusOr(const StatusCodeOr<T>& value)
      : value_{value.ok() ? StatusVariant{*value}
                          : StatusVariant{value.status_code()}} {}

  StatusOr(StatusCodeOr<T>&& value)
      : value_{value.ok() ? StatusVariant{std::move(*value)}
                          : StatusVariant{value.status_code()}} {}

  bool ok() const { return std::holds_alternative<T>(value_); }

  Status status() const {
    const auto* status = std::get_if<Status>(&value_);
    return status ? *status : Status{StatusCode::Good};
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
  using StatusVariant = std::variant<Status, T>;

  StatusVariant value_;
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
