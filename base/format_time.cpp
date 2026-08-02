#include "base/format_time.h"

#include "base/time/calendar.h"
#include "base/time/time.h"
#include <format>

std::string FormatTime(scada::Time time, int flags) {
  if (scada::base::IsNull(time))
    return {};

  if (time == scada::base::kMinTime)
    return "min";

  if (time == scada::base::kMaxTime)
    return "max";

  scada::base::Exploded e = (flags & TIME_FORMAT_UTC)
                                ? scada::base::UtcExplode(time)
                                : scada::base::LocalExplode(time);

  std::string text;

  if (flags & TIME_FORMAT_DATE)
    text = std::format("{:02}.{:02}.{:04}", e.day_of_month, e.month, e.year);

  if (flags & TIME_FORMAT_TIME) {
    std::string tmp =
        std::format("{:02}:{:02}:{:02}", e.hour, e.minute, e.second);
    if (!text.empty())
      text += L' ';
    text += tmp;
    if (flags & TIME_FORMAT_MSEC)
      text += std::format(".{:03}", e.millisecond);
  }

  return text;
}
