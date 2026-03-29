#pragma once

#include <windows.h>

namespace base {

// A simple wrapper around a Windows manual-reset event, replacing the
// ChromiumBase WaitableEvent that was removed.
class WaitableEvent {
 public:
  WaitableEvent() : event_{::CreateEventW(nullptr, TRUE, FALSE, nullptr)} {}
  ~WaitableEvent() {
    if (event_)
      ::CloseHandle(event_);
  }

  WaitableEvent(const WaitableEvent&) = delete;
  WaitableEvent& operator=(const WaitableEvent&) = delete;

  void Signal() { ::SetEvent(event_); }
  void Wait() { ::WaitForSingleObject(event_, INFINITE); }
  void Reset() { ::ResetEvent(event_); }

 private:
  HANDLE event_;
};

}  // namespace base
