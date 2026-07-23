#pragma once

#include "base/time/time.h"
#include "base/time_utils.h"

#include <chrono>
#include <memory>

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#pragma comment(lib, "psapi.lib")
#else
#include <sys/resource.h>
#include <unistd.h>
#include <fstream>
#include <string>
#endif

namespace scada::base {

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
    auto now = std::chrono::steady_clock::now();

    double cpu_usage = 0.0;
    if (last_cpu_time_ != std::chrono::steady_clock::time_point{}) {
      int64_t cpu_time_delta = cpu_time - last_cpu_time_value_;
      int64_t time_delta = InMicroseconds(now - last_cpu_time_) * 10;
      if (time_delta > 0)
        cpu_usage = cpu_time_delta * 100.0 / time_delta;
    }

    last_cpu_time_ = now;
    last_cpu_time_value_ = cpu_time;
    return cpu_usage;
#else
    // POSIX (Linux, macOS): cumulative user+system CPU time from getrusage,
    // expressed as a percentage of wall-clock elapsed since the previous call.
    struct rusage usage;
    if (getrusage(RUSAGE_SELF, &usage) != 0) {
      return 0.0;
    }
    const int64_t cpu_time =
        (static_cast<int64_t>(usage.ru_utime.tv_sec) +
         static_cast<int64_t>(usage.ru_stime.tv_sec)) *
            1000000 +
        usage.ru_utime.tv_usec + usage.ru_stime.tv_usec;
    const auto now = std::chrono::steady_clock::now();

    double cpu_usage = 0.0;
    if (last_cpu_time_ != std::chrono::steady_clock::time_point{}) {
      const int64_t cpu_time_delta = cpu_time - last_cpu_time_value_;
      const int64_t time_delta = InMicroseconds(now - last_cpu_time_);
      if (time_delta > 0)
        cpu_usage = cpu_time_delta * 100.0 / time_delta;
    }

    last_cpu_time_ = now;
    last_cpu_time_value_ = cpu_time;
    return cpu_usage;
#endif
  }

 private:
  explicit ProcessMetrics(ProcessHandle process) : process_(process) {}

  ProcessHandle process_;
  std::chrono::steady_clock::time_point last_cpu_time_;
  int64_t last_cpu_time_value_ = 0;
};

}  // namespace scada::base
