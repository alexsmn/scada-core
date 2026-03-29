#pragma once

#include "base/time/clock.h"

namespace base {

// A simple test clock that returns a fixed time. Call SetNow() to change it.
class SimpleTestClock : public Clock {
 public:
  Time Now() const override { return now_; }

  void SetNow(Time now) { now_ = now; }

  void Advance(TimeDelta delta) { now_ += delta; }

 private:
  Time now_;
};

}  // namespace base
