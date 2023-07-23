#pragma once

#include <functional>
#include <memory>

class Executor;

using ExecutorFactory = std::function<std::shared_ptr<Executor>()>;

inline ExecutorFactory MakeSingleExecutorFactory(
    std::shared_ptr<Executor> executor) {
  return [executor = std::move(executor)] { return executor; };
}
