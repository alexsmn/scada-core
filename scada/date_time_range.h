#pragma once

#include "base/interval.h"
#include "scada/date_time.h"

namespace scada {

using DateTimeRange = Interval<DateTime>;

}  // namespace scada

std::string ToString(scada::DateTime time);
std::u16string ToString16(scada::DateTime time);
