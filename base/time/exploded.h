#pragma once

namespace scada::base {

// Broken-down calendar representation of an absolute time, in either UTC or
// local time. Field ranges match the OPC UA / Chromium convention: `month` and
// `day_of_month` are 1-based, `day_of_week` is 0-based with Sunday == 0.
//
// This is a standalone struct (rather than a member of the time class) so the
// calendar-conversion free functions in base/time/calendar.h can outlive the
// concrete time representation.
struct Exploded {
  int year;
  int month;         // 1-based: 1 == January.
  int day_of_week;   // 0-based: 0 == Sunday.
  int day_of_month;  // 1-based: 1 == the first of the month.
  int hour;          // 0..23.
  int minute;        // 0..59.
  int second;        // 0..60 (60 allows a leap second).
  int millisecond;   // 0..999.

  // True when every field is within its valid range.
  bool HasValidValues() const;
};

}  // namespace scada::base
