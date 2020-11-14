#pragma once

#include "base/time/time.h"

#include <ostream>
#include <string>

namespace scada {

using DateTime = base::Time;
using DateTimeRange = std::pair<DateTime, DateTime>;

using Duration = base::TimeDelta;

}  // namespace scada

std::string ToString(scada::DateTime time);
std::wstring ToString16(scada::DateTime time);

// std::ostream& operator<<(std::ostream& stream, scada::DateTime time);
