#pragma once

#include "base/strings/string16.h"
#include "base/time/time.h"

#include <ostream>
#include <string>

namespace scada {

using DateTime = base::Time;
using DateTimeRange = std::pair<DateTime, DateTime>;

using Duration = base::TimeDelta;

}  // namespace scada

std::string ToString(scada::DateTime time);
std::u16string ToString16(scada::DateTime time);
