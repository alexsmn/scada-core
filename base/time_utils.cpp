#include "base/time_utils.h"

#include "base/string_piece_util.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_split.h"
#include "base/strings/stringprintf.h"

namespace {

#ifndef NDEBUG
base::Time FloorToMilliseconds(base::Time time) {
  return base::Time::FromDeltaSinceWindowsEpoch(
      base::TimeDelta::FromMilliseconds(
          time.ToDeltaSinceWindowsEpoch().InMilliseconds()));
}
#endif

}  // namespace

std::string SerializeToString(base::TimeDelta delta) {
  int64_t s = delta.InSeconds();
  int64_t m = s / 60;
  s = s % 60;
  int64_t h = m / 60;
  m = m % 60;
  return base::StringPrintf("%d:%02d:%02d", static_cast<int>(h),
                            static_cast<int>(m), static_cast<int>(s));
}

bool Deserialize(std::string_view str, base::TimeDelta& delta) {
  auto parts = base::SplitStringPiece(
      AsStringPiece(str), ":", base::KEEP_WHITESPACE, base::SPLIT_WANT_ALL);
  if (parts.size() != 3)
    return false;

  unsigned h, m, s;
  if (!base::StringToUint(parts[0], &h) || !base::StringToUint(parts[1], &m) ||
      !base::StringToUint(parts[2], &s)) {
    return false;
  }

  delta = base::TimeDelta::FromHours(h) + base::TimeDelta::FromMinutes(m) +
          base::TimeDelta::FromSeconds(s);
  return true;
}

std::string SerializeToString(base::Time time) {
  base::Time::Exploded e = {0};
  time.UTCExplode(&e);
  auto str =
      base::StringPrintf("%04d-%02d-%02d %02d:%02d:%02d", e.year, e.month,
                         e.day_of_month, e.hour, e.minute, e.second);

  if (e.millisecond != 0)
    str += base::StringPrintf(".%03d", e.millisecond);

#ifndef NDEBUG
  base::Time parsed_time;
  bool parse_result = Deserialize(str, parsed_time);
  assert(parse_result);
  assert(FloorToMilliseconds(time) == parsed_time);
#endif

  return str;
}

bool Deserialize(std::string_view str, base::Time& time) {
  return base::Time::FromUTCString(std::string{str}.c_str(), &time);
}
