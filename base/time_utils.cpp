#include "base/time_utils.h"

#include "base/check.h"
#include "base/format.h"
#include "base/string_util.h"
#include "base/time/calendar.h"

#include <chrono>

namespace {

#ifndef NDEBUG
scada::Time FloorToMilliseconds(scada::Time time) {
  return std::chrono::floor<std::chrono::milliseconds>(time);
}
#endif

}  // namespace

std::string SerializeToString(scada::Duration delta) {
  int64_t s = InSeconds(delta);
  int64_t m = s / 60;
  s = s % 60;
  int64_t h = m / 60;
  m = m % 60;
  return std::format("{}:{:02}:{:02}", h, m, s);
}

bool Deserialize(std::string_view str, scada::Duration& delta) {
  auto parts = SplitString(str, ":");
  if (parts.size() != 3)
    return false;

  unsigned h, m, s;
  if (!Parse(parts[0], h) || !Parse(parts[1], m) || !Parse(parts[2], s)) {
    return false;
  }

  delta = std::chrono::hours{h} + std::chrono::minutes{m} +
          std::chrono::seconds{s};
  return true;
}

std::string SerializeToString(scada::Time time) {
  scada::base::Exploded e = scada::base::UtcExplode(time);
  auto str = std::format("{:04}-{:02}-{:02} {:02}:{:02}:{:02}", e.year, e.month,
                         e.day_of_month, e.hour, e.minute, e.second);

  if (e.millisecond != 0)
    str += std::format(".{:03}", e.millisecond);

#ifndef NDEBUG
  scada::Time parsed_time;
  bool parse_result = Deserialize(str, parsed_time);
  scada::base::Check(parse_result);
  scada::base::Check(FloorToMilliseconds(time) == parsed_time);
#endif

  return str;
}

bool Deserialize(std::string_view str, scada::Time& time) {
  if (auto parsed = scada::base::TimeFromUtcString(str)) {
    time = *parsed;
    return true;
  }
  return false;
}
