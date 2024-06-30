#pragma once

#include "base/asio_executor.h"
#include "base/executor_factory.h"

inline ExecutorFactory MakeAsioExecutorFactory(
    const boost::asio::any_io_executor& executor) {
  return [executor] { return std::make_shared<AsioExecutor>(executor); };
}
