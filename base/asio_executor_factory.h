#pragma once

#include "base/asio_executor.h"
#include "base/executor_factory.h"

inline ExecutorFactory MakeAsioExecutorFactory(
    boost::asio::io_context& io_context) {
  return [&io_context] { return std::make_shared<AsioExecutor>(io_context); };
}
