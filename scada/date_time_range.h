#pragma once

#include "base/interval.h"
#include "scada/date_time.h"

namespace scada {

using TimeRange = Interval<Time>;

}  // namespace scada

std::string ToString(scada::Time time);
std::u16string ToString16(scada::Time time);
