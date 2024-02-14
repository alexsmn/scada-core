#pragma once

#include <source_location>

#if defined(NDEBUG)

template <class T>
class DebugHolder {
 public:
  explicit DebugHolder(const std::source_location& location) {}

  T get() const { return {}; }
};

#else

template <class T>
class DebugHolder {
 public:
  explicit DebugHolder(const T& value) : value_{value} {}

  const T& get() const { return value_; }

 private:
  T value_;
};

#endif
