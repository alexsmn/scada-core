#pragma once

#include "base/time/time.h"

namespace scada::base {

// Abstract clock interface. Allows injecting a mock clock for testing.
class Clock {
 public:
  virtual ~Clock() = default;

  // Returns the current time.
  virtual Time Now() const = 0;
};

}  // namespace scada::base
