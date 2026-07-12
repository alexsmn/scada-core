#pragma once

#include "base/lifetime.h"

#include <new>
#include <type_traits>
#include <utility>

#include "base/base_compat.h"
namespace scada::base {

// Stores an object of type T with static storage duration whose destructor is
// never called. Use as a function-local static to avoid global destructor
// ordering while still getting thread-safe lazy initialization.
template <typename T>
class NoDestructor {
 public:
  template <typename... Args>
  explicit NoDestructor(Args&&... args) {
    new (storage_) T(std::forward<Args>(args)...);
  }

  explicit NoDestructor(const T& x) { new (storage_) T(x); }
  explicit NoDestructor(T&& x) { new (storage_) T(std::move(x)); }

  NoDestructor(const NoDestructor&) = delete;
  NoDestructor& operator=(const NoDestructor&) = delete;

  ~NoDestructor() = default;

  const T& operator*() const SCADA_LIFETIME_BOUND { return *get(); }
  T& operator*() SCADA_LIFETIME_BOUND { return *get(); }

  const T* operator->() const SCADA_LIFETIME_BOUND { return get(); }
  T* operator->() SCADA_LIFETIME_BOUND { return get(); }

  const T* get() const SCADA_LIFETIME_BOUND {
    return reinterpret_cast<const T*>(storage_);
  }
  T* get() SCADA_LIFETIME_BOUND { return reinterpret_cast<T*>(storage_); }

 private:
  alignas(T) char storage_[sizeof(T)];
};

}  // namespace scada::base
