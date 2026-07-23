#pragma once

#include "base/time/calendar.h"
#include "base/time/time.h"

// Example: "Tue, 15 Nov 1994 12:45:26 GMT"
inline scada::Time TestTimeFromString(const char* time_string) {
  return scada::base::TimeFromString(time_string, /*is_local=*/true)
      .value_or(scada::base::kNullTime);
}
