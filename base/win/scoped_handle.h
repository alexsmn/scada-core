#pragma once

#include <windows.h>

namespace base {
namespace win {

class ScopedHandle {
 public:
  ScopedHandle() = default;
  explicit ScopedHandle(HANDLE handle) : handle_{handle} {}

  ~ScopedHandle() { Close(); }

  ScopedHandle(const ScopedHandle&) = delete;
  ScopedHandle& operator=(const ScopedHandle&) = delete;

  ScopedHandle(ScopedHandle&& other) noexcept : handle_{other.handle_} {
    other.handle_ = INVALID_HANDLE_VALUE;
  }

  ScopedHandle& operator=(ScopedHandle&& other) noexcept {
    if (this != &other) {
      Close();
      handle_ = other.handle_;
      other.handle_ = INVALID_HANDLE_VALUE;
    }
    return *this;
  }

  void Set(HANDLE handle) {
    Close();
    handle_ = handle;
  }

  HANDLE Get() const { return handle_; }

  bool IsValid() const {
    return handle_ != INVALID_HANDLE_VALUE && handle_ != nullptr;
  }

  HANDLE Take() {
    HANDLE h = handle_;
    handle_ = INVALID_HANDLE_VALUE;
    return h;
  }

 private:
  void Close() {
    if (IsValid()) {
      CloseHandle(handle_);
      handle_ = INVALID_HANDLE_VALUE;
    }
  }

  HANDLE handle_ = INVALID_HANDLE_VALUE;
};

}  // namespace win
}  // namespace base
