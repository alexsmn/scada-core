#pragma once
#include <windows.h>

namespace base {
namespace win {

template <typename T>
class ScopedGdiObject {
 public:
  ScopedGdiObject() = default;
  explicit ScopedGdiObject(T handle) : handle_(handle) {}
  ~ScopedGdiObject() { Close(); }

  ScopedGdiObject(const ScopedGdiObject&) = delete;
  ScopedGdiObject& operator=(const ScopedGdiObject&) = delete;

  ScopedGdiObject(ScopedGdiObject&& other) noexcept : handle_(other.handle_) {
    other.handle_ = nullptr;
  }

  T get() const { return handle_; }
  operator T() const { return handle_; }

  void reset(T handle = nullptr) {
    Close();
    handle_ = handle;
  }

 private:
  void Close() {
    if (handle_) {
      DeleteObject(handle_);
      handle_ = nullptr;
    }
  }
  T handle_ = nullptr;
};

using ScopedBitmap = ScopedGdiObject<HBITMAP>;
using ScopedRegion = ScopedGdiObject<HRGN>;
using ScopedHFONT = ScopedGdiObject<HFONT>;
using ScopedHPEN = ScopedGdiObject<HPEN>;
using ScopedHBRUSH = ScopedGdiObject<HBRUSH>;

}  // namespace win
}  // namespace base
