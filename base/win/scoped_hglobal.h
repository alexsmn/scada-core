#pragma once
#include <windows.h>

namespace base {
namespace win {

template <typename T>
class ScopedHGlobal {
 public:
  explicit ScopedHGlobal(HGLOBAL handle)
      : handle_(handle), ptr_(static_cast<T>(::GlobalLock(handle))) {}
  ~ScopedHGlobal() {
    if (handle_) ::GlobalUnlock(handle_);
  }

  ScopedHGlobal(const ScopedHGlobal&) = delete;
  ScopedHGlobal& operator=(const ScopedHGlobal&) = delete;

  T get() const { return ptr_; }
  operator T() const { return ptr_; }
  size_t Size() const { return handle_ ? ::GlobalSize(handle_) : 0; }

 private:
  HGLOBAL handle_;
  T ptr_;
};

}  // namespace win
}  // namespace base
