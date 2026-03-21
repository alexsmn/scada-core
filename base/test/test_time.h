#pragma once

#include "base/time/time.h"
#include <cassert>

// Example: "Tue, 15 Nov 1994 12:45:26 GMT"
inline base::Time TestTimeFromString(const char* time_string) {
  base::Time time;
  base::Time::FromString(time_string, &time);
  return time;
}
