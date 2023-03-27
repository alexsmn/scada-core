#pragma once

#include <base/time/time.h>
#include <cassert>

inline base::Time TestTimeFromString(const char* time_string) {
  base::Time time;
  base::Time::FromString(time_string, &time);
  return time;
}
