#pragma once

#include "base/awaitable.h"
#include "scada/status_exception.h"
#include "scada/status_or.h"

namespace scada {

inline void ThrowIfBad(Status status) {
  if (status.bad()) {
    throw status_exception{std::move(status)};
  }
}

inline void ThrowIfBad(StatusCode status_code) {
  if (IsBad(status_code)) {
    throw status_exception{status_code};
  }
}

template <class T>
T ValueOrThrow(StatusOr<T>&& value) {
  if (!value.ok()) {
    throw status_exception{std::move(value).status()};
  }
  return std::move(*value);
}

template <class T>
T ValueOrThrow(const StatusOr<T>& value) {
  if (!value.ok()) {
    throw status_exception{value.status()};
  }
  return *value;
}

}  // namespace scada
