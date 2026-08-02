#include "base/process/process_start_time.h"

#include <charconv>
#include <chrono>
#include <string>
#include <string_view>

#ifdef _WIN32
#include <windows.h>

#include "base/time/time_wire_codec.h"
#elif defined(__APPLE__)
#include <sys/sysctl.h>
#include <sys/types.h>
#include <unistd.h>
#else
#include <unistd.h>

#include <fstream>
#include <ios>
#include <sstream>
#endif

namespace scada::base {

namespace {

// Parses the token at `index` (0-based) of a whitespace-separated list as an
// integer. Returns nullopt when the token is missing or malformed.
std::optional<std::int64_t> ParseTokenAt(std::string_view text,
                                         std::size_t index) {
  constexpr std::string_view kSpace = " \t";
  for (std::size_t i = 0;; ++i) {
    const std::size_t begin = text.find_first_not_of(kSpace);
    if (begin == std::string_view::npos) {
      return std::nullopt;
    }
    text.remove_prefix(begin);

    const std::size_t end = text.find_first_of(kSpace);
    if (i == index) {
      const std::string_view token = text.substr(0, end);
      std::int64_t value = 0;
      const auto result =
          std::from_chars(token.data(), token.data() + token.size(), value);
      if (result.ec != std::errc{} ||
          result.ptr != token.data() + token.size()) {
        return std::nullopt;
      }
      return value;
    }
    if (end == std::string_view::npos) {
      return std::nullopt;
    }
    text.remove_prefix(end);
  }
}

// Seconds since the Unix epoch at which the system booted, from the `btime`
// line of /proc/stat.
std::optional<std::int64_t> ParseBootTimeSeconds(std::string_view proc_stat) {
  constexpr std::string_view kPrefix = "btime ";
  while (!proc_stat.empty()) {
    const std::size_t end = proc_stat.find('\n');
    const std::string_view line = proc_stat.substr(0, end);
    if (line.starts_with(kPrefix)) {
      return ParseTokenAt(line.substr(kPrefix.size()), 0);
    }
    if (end == std::string_view::npos) {
      break;
    }
    proc_stat.remove_prefix(end + 1);
  }
  return std::nullopt;
}

#ifdef _WIN32

// Windows reports the creation time of a process handle directly, as a FILETIME
// (100-ns ticks since the 1601 epoch).
std::optional<Time> QueryProcessStartTime() {
  FILETIME creation_time, exit_time, kernel_time, user_time;
  if (!::GetProcessTimes(::GetCurrentProcess(), &creation_time, &exit_time,
                         &kernel_time, &user_time)) {
    return std::nullopt;
  }
  return DecodeFileTime(creation_time);
}

#elif defined(__APPLE__)

// macOS (and the BSDs) expose the process start time as an absolute timeval in
// the kinfo_proc record returned by the KERN_PROC_PID sysctl.
std::optional<Time> QueryProcessStartTime() {
  struct kinfo_proc info = {};
  std::size_t size = sizeof(info);
  int mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_PID, ::getpid()};
  if (::sysctl(mib, 4, &info, &size, nullptr, 0) != 0 || size == 0) {
    return std::nullopt;
  }
  const auto& started = info.kp_proc.p_starttime;
  return Time{std::chrono::seconds{started.tv_sec} +
              std::chrono::microseconds{started.tv_usec}};
}

#else

// Reads a whole /proc file. They report a size of 0, so the usual seek-to-end
// sizing does not work; read to EOF instead.
std::optional<std::string> ReadProcFile(const char* path) {
  std::ifstream file{path, std::ios::binary};
  if (!file) {
    return std::nullopt;
  }
  std::ostringstream contents;
  contents << file.rdbuf();
  if (file.bad()) {
    return std::nullopt;
  }
  return std::move(contents).str();
}

// Linux reports the process start as clock ticks since boot, so it has to be
// rebased onto the boot wall-clock time; see ParseProcProcessStartTime.
std::optional<Time> QueryProcessStartTime() {
  const std::optional<std::string> proc_self_stat =
      ReadProcFile("/proc/self/stat");
  const std::optional<std::string> proc_stat = ReadProcFile("/proc/stat");
  if (!proc_self_stat || !proc_stat) {
    return std::nullopt;
  }
  return ParseProcProcessStartTime(*proc_self_stat, *proc_stat,
                                   ::sysconf(_SC_CLK_TCK));
}

#endif

}  // namespace

std::optional<Time> ParseProcProcessStartTime(std::string_view proc_self_stat,
                                              std::string_view proc_stat,
                                              long ticks_per_second) {
  if (ticks_per_second <= 0) {
    return std::nullopt;
  }

  // Field 2 of /proc/self/stat is the executable name in parentheses and may
  // itself contain spaces and parentheses, so tokenizing has to start after its
  // *last* ')'. What follows is field 3 (state), which makes field 22
  // (starttime) the token at 0-based index 19.
  const std::size_t comm_end = proc_self_stat.rfind(')');
  if (comm_end == std::string_view::npos) {
    return std::nullopt;
  }
  const std::optional<std::int64_t> start_ticks =
      ParseTokenAt(proc_self_stat.substr(comm_end + 1), 19);
  if (!start_ticks || *start_ticks < 0) {
    return std::nullopt;
  }

  const std::optional<std::int64_t> boot_seconds =
      ParseBootTimeSeconds(proc_stat);
  if (!boot_seconds) {
    return std::nullopt;
  }

  const std::int64_t since_boot_us =
      *start_ticks * kMicrosecondsPerSecond / ticks_per_second;
  return Time{std::chrono::seconds{*boot_seconds} +
              std::chrono::microseconds{since_boot_us}};
}

Time GetCurrentProcessStartTime() {
  // Function-local static: the OS query runs once, on the first call. Time is
  // trivially destructible, so this registers no exit-time destructor.
  static const Time start_time =
      QueryProcessStartTime().value_or(scada::base::NowUtc());
  return start_time;
}

}  // namespace scada::base
