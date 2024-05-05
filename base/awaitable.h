#pragma once

#include "base/net_executor_adapter.h"

#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>

template <typename T>
using Awaitable = boost::asio::awaitable<T>;

template <typename A>
inline void CoSpawn(const std::shared_ptr<Executor>& executor, A awaitable) {
  boost::asio::co_spawn(NetExecutorAdapter{executor}, std::move(awaitable),
                        boost::asio::detached);
}
