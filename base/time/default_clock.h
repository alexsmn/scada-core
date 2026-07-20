#pragma once

#include "base/time/clock.h"

namespace scada::base {

// Default clock implementation using base::Time::Now().
class DefaultClock : public Clock {
 public:
  static DefaultClock* GetInstance() {
    static DefaultClock instance;
    return &instance;
  }

  Time Now() const override { return Time::Now(); }
};

}  // namespace scada::base
