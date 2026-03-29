#pragma once
#include <windows.h>

namespace base {
namespace win {

class ScopedSelectObject {
 public:
  ScopedSelectObject(HDC hdc, HGDIOBJ object)
      : hdc_(hdc), old_object_(::SelectObject(hdc, object)) {}
  ~ScopedSelectObject() { ::SelectObject(hdc_, old_object_); }

  ScopedSelectObject(const ScopedSelectObject&) = delete;
  ScopedSelectObject& operator=(const ScopedSelectObject&) = delete;

 private:
  HDC hdc_;
  HGDIOBJ old_object_;
};

}  // namespace win
}  // namespace base
