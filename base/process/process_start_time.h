#pragma once

#include "base/time/time.h"

#include <optional>
#include <string_view>

namespace scada::base {

// The wall-clock instant (UTC) at which the current process was started, as
// reported by the OS: GetProcessTimes on Windows, /proc/self/stat plus
// /proc/stat's btime on Linux, and the KERN_PROC_PID sysctl on macOS. The
// query runs once and the result is cached, so repeated calls are cheap and
// always yield the same instant.
//
// Where the OS query is unavailable or fails, this falls back to the wall clock
// at the first call — a slight overestimate rather than a wrong or null value.
// The result is therefore always a plausible timestamp at or before "now", and
// never the null sentinel.
Time GetCurrentProcessStartTime();

// The Linux computation, as a pure function of its inputs: `proc_self_stat` is
// the contents of /proc/self/stat (whose field 22 is the start time in clock
// ticks since boot), `proc_stat` the contents of /proc/stat (whose `btime` line
// is the boot time in seconds since the Unix epoch), and `ticks_per_second` the
// value of sysconf(_SC_CLK_TCK). Returns nullopt if either input is malformed.
//
// Defined on every platform, not just Linux, so the parsing that the deployed
// Linux tiers depend on is unit-testable from any development host.
std::optional<Time> ParseProcProcessStartTime(std::string_view proc_self_stat,
                                              std::string_view proc_stat,
                                              long ticks_per_second);

}  // namespace scada::base
