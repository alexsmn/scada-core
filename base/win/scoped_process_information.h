#pragma once
#include <windows.h>

namespace base {
namespace win {

class ScopedProcessInformation {
 public:
  ScopedProcessInformation() { memset(&info_, 0, sizeof(info_)); }
  explicit ScopedProcessInformation(const PROCESS_INFORMATION& info)
      : info_(info) {}
  ~ScopedProcessInformation() {
    if (info_.hProcess) ::CloseHandle(info_.hProcess);
    if (info_.hThread) ::CloseHandle(info_.hThread);
  }

  ScopedProcessInformation(const ScopedProcessInformation&) = delete;
  ScopedProcessInformation& operator=(const ScopedProcessInformation&) = delete;

  HANDLE process_handle() const { return info_.hProcess; }
  HANDLE thread_handle() const { return info_.hThread; }
  DWORD process_id() const { return info_.dwProcessId; }
  DWORD thread_id() const { return info_.dwThreadId; }

  PROCESS_INFORMATION* Receive() { return &info_; }

 private:
  PROCESS_INFORMATION info_;
};

}  // namespace win
}  // namespace base
