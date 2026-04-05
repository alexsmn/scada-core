#pragma once

#include <optional>

template <class T>
inline const T* OptionalToPtr(const std::optional<T>& opt) {
  return opt.has_value() ? &opt.value() : nullptr;
}

template <class T>
inline T* OptionalToPtr(std::optional<T>& opt) {
  return opt.has_value() ? &opt.value() : nullptr;
}

template <class T>
inline std::optional<T> OptionalFromPtr(const T* ptr) {
  return ptr ? std::optional<T>(*ptr) : std::nullopt;
}
