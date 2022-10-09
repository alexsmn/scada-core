#pragma once

#include "core/status.h"

#include <cassert>

namespace scada {

template <class T>
using StatusOr = std::pair<Status, T>;

template <class T>
using StatusCodeOr = std::pair<StatusCode, T>;

template <class T>
inline auto MakeStatusCodeOr(T&& value) {
  return StatusCodeOr<std::decay_t<T>>{scada::StatusCode::Good,
                                       std::move(value)};
}

template <class T>
inline auto MakeStatusCodeOr(scada::StatusCode error_code) {
  assert(scada::IsBad(error_code));
  return StatusCodeOr<T>{error_code, {}};
}

template <class T, class F>
inline auto MapStatusCodeOr(const scada::StatusCodeOr<T>& value, F&& f) {
  using R = std::invoke_result_t<F, T>;
  return scada::IsGood(value.first) ? f(value.second) : R{value.first, {}};
}

template <class T, class F>
inline auto MapStatusCodeOr(scada::StatusCodeOr<T>&& value, F&& f) {
  using R = std::invoke_result_t<F, T>;
  return scada::IsGood(value.first) ? f(std::move(value.second))
                                    : R{value.first, {}};
}

template <class F>
inline auto TransformStatusCodeOr(F&& f) {
  return [&](auto&& value) {
    return MapStatusCodeOr(std::forward<decltype(value)>(value),
                           std::forward<F>(f));
  };
}

}  // namespace scada
