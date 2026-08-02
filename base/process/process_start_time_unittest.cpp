#include "base/process/process_start_time.h"

#include "base/time/time.h"

#include <gtest/gtest.h>

#include <chrono>
#include <cstdint>
#include <string>
#include <string_view>

namespace scada::base {

namespace {

TEST(ProcessStartTimeTest, ReportsPlausibleInstant) {
  const Time start_time = GetCurrentProcessStartTime();

  // A real instant, not the null sentinel or the default-constructed Unix
  // epoch, and not in the future.
  EXPECT_FALSE(IsNull(start_time));
  EXPECT_GT(start_time, Time{});
  const Time now = NowUtc();
  EXPECT_LE(start_time, now);
  // The test process cannot have started long ago. Generous enough to survive a
  // slow build machine or a debugger session, tight enough to catch an epoch or
  // unit error (which lands decades off, not hours).
  EXPECT_GT(start_time, now - std::chrono::hours{24});
}

TEST(ProcessStartTimeTest, IsStableAcrossCalls) {
  EXPECT_EQ(GetCurrentProcessStartTime(), GetCurrentProcessStartTime());
}

// --- The Linux /proc computation -------------------------------------------
// Runs on every host, so the path the deployed tiers take is covered from a
// macOS or Windows workstation too.

// A /proc/self/stat line, abridged after field 22 (starttime); the fields
// before it are the real shapes the parser has to skip over.
std::string MakeProcSelfStat(std::string_view comm, std::int64_t start_ticks) {
  std::string stat = "1234 (";
  stat += comm;
  stat += ") S 1 1234 1234 0 -1 4194560 900 0 11 0 42 7 0 0 20 0 14 0 ";
  stat += std::to_string(start_ticks);
  stat += " 226643968 2519 18446744073709551615 4194304 5242880\n";
  return stat;
}

constexpr std::string_view kProcStat =
    "cpu  100 0 200 3000 0 0 0 0 0 0\n"
    "cpu0 50 0 100 1500 0 0 0 0 0 0\n"
    "intr 12345 0 0\n"
    "ctxt 987654\n"
    "btime 1753400000\n"
    "processes 4321\n"
    "procs_running 2\n";

TEST(ProcessStartTimeTest, ParsesProcInputs) {
  // 1753400000 (boot) + 3000 ticks / 100 Hz = 30 s after boot.
  const auto start_time = ParseProcProcessStartTime(
      MakeProcSelfStat("scada-proxy", 3000), kProcStat, 100);

  ASSERT_TRUE(start_time.has_value());
  EXPECT_EQ(*start_time, Time{std::chrono::seconds{1753400030}});
}

// Field 2 is the raw executable name: it can contain spaces and parentheses,
// so the scan must resume after its *last* ')', not the first.
TEST(ProcessStartTimeTest, ParsesProcInputsWithAwkwardExecutableName) {
  const auto start_time = ParseProcProcessStartTime(
      MakeProcSelfStat("weird (name) x", 3000), kProcStat, 100);

  ASSERT_TRUE(start_time.has_value());
  EXPECT_EQ(*start_time, Time{std::chrono::seconds{1753400030}});
}

// Sub-second resolution survives: ticks are not truncated to whole seconds.
TEST(ProcessStartTimeTest, ParsesProcInputsWithSubSecondTicks) {
  const auto start_time = ParseProcProcessStartTime(
      MakeProcSelfStat("scada-proxy", 3025), kProcStat, 100);

  ASSERT_TRUE(start_time.has_value());
  EXPECT_EQ(*start_time, Time{std::chrono::seconds{1753400030} +
                              std::chrono::milliseconds{250}});
}

TEST(ProcessStartTimeTest, RejectsMalformedProcInputs) {
  const std::string stat = MakeProcSelfStat("scada-proxy", 3000);

  // No btime line: nothing to rebase the tick count onto.
  EXPECT_FALSE(
      ParseProcProcessStartTime(stat, "cpu 1 2 3\nctxt 4\n", 100).has_value());
  // Not a /proc/self/stat line at all (no comm field).
  EXPECT_FALSE(
      ParseProcProcessStartTime("garbage", kProcStat, 100).has_value());
  // Truncated before field 22.
  EXPECT_FALSE(ParseProcProcessStartTime("1234 (x) S 1 1234", kProcStat, 100)
                   .has_value());
  // Nonsensical clock rate.
  EXPECT_FALSE(ParseProcProcessStartTime(stat, kProcStat, 0).has_value());
}

}  // namespace

}  // namespace scada::base
