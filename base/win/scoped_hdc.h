#pragma once
#include <windows.h>

namespace base {
namespace win {

class ScopedGetDC {
 public:
  explicit ScopedGetDC(HWND hwnd) : hwnd_(hwnd), hdc_(::GetDC(hwnd)) {}
  ~ScopedGetDC() { if (hdc_) ::ReleaseDC(hwnd_, hdc_); }

  ScopedGetDC(const ScopedGetDC&) = delete;
  ScopedGetDC& operator=(const ScopedGetDC&) = delete;

  HDC Get() const { return hdc_; }
  operator HDC() const { return hdc_; }

 private:
  HWND hwnd_;
  HDC hdc_;
};

class ScopedCreateDC {
 public:
  ScopedCreateDC() = default;
  explicit ScopedCreateDC(HDC hdc) : hdc_(hdc) {}
  ~ScopedCreateDC() { if (hdc_) ::DeleteDC(hdc_); }

  ScopedCreateDC(const ScopedCreateDC&) = delete;
  ScopedCreateDC& operator=(const ScopedCreateDC&) = delete;

  ScopedCreateDC(ScopedCreateDC&& other) noexcept : hdc_(other.hdc_) {
    other.hdc_ = nullptr;
  }

  HDC Get() const { return hdc_; }
  operator HDC() const { return hdc_; }

  void Set(HDC hdc) {
    if (hdc_) ::DeleteDC(hdc_);
    hdc_ = hdc;
  }

 private:
  HDC hdc_ = nullptr;
};

}  // namespace win
}  // namespace base
