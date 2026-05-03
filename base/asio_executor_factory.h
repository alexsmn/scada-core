#pragma once

#include "base/asio_executor.h"
#include "base/any_executor.h"

inline AnyExecutorFactory MakeAsioExecutorFactory(
    const boost::asio::any_io_executor& executor) {
  return [executor] { return std::make_shared<AsioExecutor>(executor); };
}
