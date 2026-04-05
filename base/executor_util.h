#pragma once

#include "base/debug_holder.h"

#include <boost/asio/post.hpp>
#include <boost/asio/steady_timer.hpp>
#include <source_location>

template <class Ex, class Task>
inline void PostDelayedTask(
    const Ex& executor,
    std::chrono::nanoseconds delay,
    Task&& task,
    const std::source_location& location = std::source_location::current()) {
  if (delay == std::chrono::nanoseconds::zero()) {
    boost::asio::post(executor, std::forward<Task>(task));
    return;
  }

  auto timer = std::make_shared<boost::asio::steady_timer>(executor);
  timer->expires_after(delay);
  timer->async_wait([timer, captured_task = std::forward<Task>(task),
                     captured_location = DebugHolder{location}](
                        boost::system::error_code ec) mutable {
    if (ec != boost::asio::error::operation_aborted) {
      std::move(captured_task)();
    }
  });
}
