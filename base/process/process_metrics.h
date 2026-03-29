#pragma once

#include "base/time/time.h"

#include <memory>

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#pragma comment(lib, "psapi.lib")
#else
#include <unistd.h>
#include <fstream>
#include <string>
#endif

namespace base {

#ifdef _WIN32
using ProcessHandle = HANDLE;

inline ProcessHandle GetCurrentProcessHandle() {
  return ::GetCurrentProcess();
}

struct SystemMemoryInfoKB {
  int total = 0;
  int avail_phys = 0;
};

inline bool GetSystemMemoryInfo(SystemMemoryInfoKB* info) {
  MEMORYSTATUSEX mem_status;
  mem_status.dwLength = sizeof(mem_status);
  if (!GlobalMemoryStatusEx(&mem_status))
    return false;
  info->total = static_cast<int>(mem_status.ullTotalPhys / 1024);
  info->avail_phys = static_cast<int>(mem_status.ullAvailPhys / 1024);
  return true;
}
#else
using ProcessHandle = pid_t;

inline ProcessHandle GetCurrentProcessHandle() {
  return getpid();
}

struct SystemMemoryInfoKB {
  int total = 0;
  int avail_phys = 0;
};

inline bool GetSystemMemoryInfo(SystemMemoryInfoKB* /*info*/) {
  return false;
}
#endif

class ProcessMetrics {
 public:
  static std::unique_ptr<ProcessMetrics> CreateProcessMetrics(
      ProcessHandle process) {
    return std::unique_ptr<ProcessMetrics>(new ProcessMetrics(process));
  }

  double GetPlatformIndependentCPUUsage() {
#ifdef _WIN32
    FILETIME creation_time, exit_time, kernel_time, user_time;
    if (!GetProcessTimes(process_, &creation_time, &exit_time, &kernel_time,
                         &user_time)) {
      return 0.0;
    }

    auto FileTimeToInt64 = [](const FILETIME& ft) -> int64_t {
      return (static_cast<int64_t>(ft.dwHighDateTime) << 32) | ft.dwLowDateTime;
    };

    int64_t cpu_time =
        FileTimeToInt64(kernel_time) + FileTimeToInt64(user_time);
    auto now = TimeTicks::Now();

    double cpu_usage = 0.0;
    if (!last_cpu_time_.is_null()) {
      int64_t cpu_time_delta = cpu_time - last_cpu_time_value_;
      int64_t time_delta = (now - last_cpu_time_).InMicroseconds() * 10;
      if (time_delta > 0)
        cpu_usage = cpu_time_delta * 100.0 / time_delta;
    }

    last_cpu_time_ = now;
    last_cpu_time_value_ = cpu_time;
    return cpu_usage;
#else
    return 0.0;
#endif
  }

 private:
  explicit ProcessMetrics(ProcessHandle process) : process_(process) {}

  ProcessHandle process_;
  TimeTicks last_cpu_time_;
  int64_t last_cpu_time_value_ = 0;
};

}  // namespace base
