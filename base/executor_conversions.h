#pragma once

#include "base/any_executor.h"
#include "base/asio_executor.h"
#include "base/executor_adapter.h"
#include "base/thread_executor.h"

#include <memory>

inline std::shared_ptr<Executor> MakeLegacyExecutor(
    const AnyExecutor& executor) {
  return std::make_shared<AsioExecutor>(executor);
}

inline AnyExecutor MakeAnyExecutor(std::shared_ptr<Executor> executor) {
  return AnyExecutor{ExecutorAdapter{std::move(executor)}};
}

inline AnyExecutor MakeThreadAnyExecutor() {
  return MakeAnyExecutor(std::make_shared<ThreadExecutor>());
}
