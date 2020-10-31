#pragma once

#include "base/executor.h"

namespace testing {

class TestExecutor : public Executor {
 public:
  virtual void PostDelayedTask(Duration delay, Task task) override { task(); }

  virtual size_t GetTaskCount() const override { return 0; }
};

}  // namespace testing
