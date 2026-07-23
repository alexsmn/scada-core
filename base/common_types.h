#pragma once

#include <chrono>

using Clock = std::chrono::steady_clock;
using TimePoint = Clock::time_point;

// Note: there is deliberately no global `Duration` alias. The name `Duration`
// belongs to the OPC UA `Duration` DataType (`scada::Duration =
// std::chrono::microseconds`, see core/scada/date_time.h). A global
// steady-clock `Duration` (`std::chrono::nanoseconds`) would be shadowed by it
// inside every `namespace scada::` scope — and because both are now
// `std::chrono` durations, the two would compile interchangeably while silently
// meaning different precisions. Spell the steady-clock duration as
// `Clock::duration`.
