#pragma once

#include <boost/asio/steady_timer.hpp>
#include <boost/assert/source_location.hpp>

template <class Task>
inline void PostDelayedTask(
    boost::asio::io_context& io_context,
    std::chrono::nanoseconds delay,
    Task&& task,
    const boost::source_location& location = BOOST_CURRENT_LOCATION) {
  if (delay == std::chrono::nanoseconds::zero()) {
    io_context.post(std::forward<Task>(task));
    return;
  }

  auto timer = std::make_shared<boost::asio::steady_timer>(io_context);
  timer->expires_from_now(delay);
  timer->async_wait([timer, task = std::forward<Task>(task)
#ifndef NDEBUG
                                ,
                     location
#endif
  ](boost::system::error_code ec) mutable {
    if (ec != boost::asio::error::operation_aborted)
      task();
  });
}
