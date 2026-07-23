#include "base/time/time.h"

#include "base/check.h"
#include "base/time/calendar.h"

#include <chrono>
#include <iomanip>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>

namespace scada::base {

namespace {

bool is_in_range(int value, int lo, int hi) {
  return lo <= value && value <= hi;
}

}  // namespace

bool Exploded::HasValidValues() const {
  return is_in_range(month, 1, 12) && is_in_range(day_of_week, 0, 6) &&
         is_in_range(day_of_month, 1, 31) && is_in_range(hour, 0, 23) &&
         is_in_range(minute, 0, 59) && is_in_range(second, 0, 60) &&
         is_in_range(millisecond, 0, 999);
}

bool ExplodedMostlyEquals(const Exploded& lhs, const Exploded& rhs) {
  return lhs.year == rhs.year && lhs.month == rhs.month &&
         lhs.day_of_month == rhs.day_of_month && lhs.hour == rhs.hour &&
         lhs.minute == rhs.minute && lhs.second == rhs.second &&
         lhs.millisecond == rhs.millisecond;
}

// UTC calendar conversions — pure std::chrono, no tzdb required. libc++ ships no
// time-zone database, so only these UTC paths can be portable; the local-time
// paths (LocalExplode/FromLocalExploded) stay on the OS in the platform sources.

Exploded UtcExplode(Time time) {
  using namespace std::chrono;
  const sys_days days = floor<::std::chrono::days>(time);
  const year_month_day ymd{days};
  const hh_mm_ss hms{floor<milliseconds>(time - days)};

  Exploded exploded;
  exploded.year = static_cast<int>(ymd.year());
  exploded.month = static_cast<int>(static_cast<unsigned>(ymd.month()));
  exploded.day_of_month = static_cast<int>(static_cast<unsigned>(ymd.day()));
  exploded.day_of_week = static_cast<int>(weekday{days}.c_encoding());
  exploded.hour = static_cast<int>(hms.hours().count());
  exploded.minute = static_cast<int>(hms.minutes().count());
  exploded.second = static_cast<int>(hms.seconds().count());
  exploded.millisecond = static_cast<int>(hms.subseconds().count());
  return exploded;
}

std::optional<Time> FromUtcExploded(const Exploded& exploded) {
  using namespace std::chrono;
  const year_month_day ymd{year{exploded.year},
                           month{static_cast<unsigned>(exploded.month)},
                           day{static_cast<unsigned>(exploded.day_of_month)}};
  if (!ymd.ok())
    return std::nullopt;

  const auto time_of_day = hours{exploded.hour} + minutes{exploded.minute} +
                           seconds{exploded.second} +
                           milliseconds{exploded.millisecond};
  const Time result = time_point_cast<Duration>(sys_days{ymd} + time_of_day);

  // Reject component combinations that don't round-trip (parity with the old
  // implementation, e.g. hour == 25 or a normalised out-of-range field).
  if (!ExplodedMostlyEquals(UtcExplode(result), exploded))
    return std::nullopt;
  return result;
}

Time LocalMidnight(Time time) {
  Exploded exploded = LocalExplode(time);
  exploded.hour = 0;
  exploded.minute = 0;
  exploded.second = 0;
  exploded.millisecond = 0;
  if (std::optional<Time> midnight = FromLocalExploded(exploded))
    return *midnight;
  base::Check(false, "LocalMidnight failed");
  return kNullTime;
}

std::optional<Time> TimeFromString(std::string_view str, bool is_local) {
  std::tm tm = {};
  std::string_view input(str);
  bool has_timezone = false;
  bool is_utc = false;

  // Skip optional day-of-week prefix like "Tue, ".
  if (input.size() > 4 && input[3] == ',') {
    input.remove_prefix(4);
    while (!input.empty() && input.front() == ' ')
      input.remove_prefix(1);
  }

  std::istringstream ss{std::string(input)};

  // Try ISO 8601 with 'T': "2021-11-07T12:41:21"
  ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
  if (ss.fail()) {
    // Try ISO 8601 with space: "2004-11-15 10:00:00"
    ss.clear();
    ss.str(std::string(input));
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
  }
  if (ss.fail()) {
    // Try RFC 822-like: "15 Nov 2004 10:00:00 [UTC|GMT]"
    ss.clear();
    ss.str(std::string(input));
    ss >> std::get_time(&tm, "%d %b %Y %H:%M:%S");
  }

  if (ss.fail())
    return std::nullopt;

  // Parse optional fractional seconds: ".500"
  int millisecond = 0;
  if (ss.peek() == '.') {
    ss.get();
    int frac = 0;
    int digits = 0;
    while (ss.peek() >= '0' && ss.peek() <= '9' && digits < 3) {
      frac = frac * 10 + (ss.get() - '0');
      ++digits;
    }
    // Pad to 3 digits (e.g., ".5" -> 500, ".50" -> 500).
    for (; digits < 3; ++digits)
      frac *= 10;
    millisecond = frac;
    // Skip remaining fractional digits.
    while (ss.peek() >= '0' && ss.peek() <= '9')
      ss.get();
  }

  // Check for timezone suffix.
  std::string tz;
  ss >> tz;
  if (tz == "GMT" || tz == "UTC") {
    has_timezone = true;
    is_utc = true;
  }

  // Determine if this should be treated as UTC. When is_local is false always
  // treat as UTC; otherwise treat as UTC only if the timezone suffix says so.
  const bool treat_as_utc = !is_local || (has_timezone && is_utc);

  Exploded exploded = {};
  exploded.year = tm.tm_year + 1900;
  exploded.month = tm.tm_mon + 1;
  exploded.day_of_month = tm.tm_mday;
  exploded.hour = tm.tm_hour;
  exploded.minute = tm.tm_min;
  exploded.second = tm.tm_sec;
  exploded.millisecond = millisecond;

  return treat_as_utc ? FromUtcExploded(exploded) : FromLocalExploded(exploded);
}

}  // namespace scada::base
