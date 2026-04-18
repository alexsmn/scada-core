#pragma once

#include "net/net_executor_adapter.h"

#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>

template <typename T>
using Awaitable = boost::asio::awaitable<T>;

// Accept a coroutine factory, not a pre-created awaitable. Calling
// `CoSpawn(executor, lambda())` constructs the coroutine before `co_spawn`
// takes ownership of it, which has caused resumption/lifetime corruption in
// production request handlers. Keep this helper forwarding the callable itself
// so coroutine construction stays inside `boost::asio::co_spawn`.
template <class F>
inline void CoSpawn(const std::shared_ptr<Executor>& executor, F&& fn) {
  boost::asio::co_spawn(NetExecutorAdapter{executor}, std::forward<F>(fn),
                        boost::asio::detached);
}
