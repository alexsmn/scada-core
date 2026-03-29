#pragma once

#include <windows.h>

class WaitCursor {
 public:
  WaitCursor()
      : old_cursor_(SetCursor(LoadCursor(NULL, IDC_WAIT))) {
  }

  ~WaitCursor() {
    SetCursor(old_cursor_);
  }

  WaitCursor(const WaitCursor&) = delete;
  WaitCursor& operator=(const WaitCursor&) = delete;

 private:
  HCURSOR old_cursor_;
};
