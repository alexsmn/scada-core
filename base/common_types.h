#pragma once

#include <chrono>

using Clock = std::chrono::steady_clock;
using TimePoint = Clock::time_point;

// Note: there is deliberately no global `Duration` alias. The name `Duration`
// belongs to the OPC UA `Duration` DataType (`scada::Duration = base::TimeDelta`,
// see core/scada/date_time.h); a global steady-clock `Duration` would be shadowed
// by it inside every `namespace scada::` scope and silently mean the wrong type.
// Spell the steady-clock duration as `Clock::duration`.
