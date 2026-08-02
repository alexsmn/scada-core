#pragma once

#include "base/any_executor.h"
#include "base/cancelation.h"

#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/use_future.hpp>
#include <chrono>
#include <functional>
#include <future>
#include <type_traits>

template <typename T>
using Awaitable = boost::asio::awaitable<T>;

// Accept a coroutine factory, not a pre-created awaitable. Calling
// `CoSpawn(executor, lambda())` constructs the coroutine before `co_spawn`
// takes ownership of it, which has caused resumption/lifetime corruption in
// production request handlers. Keep this helper forwarding the callable itself
// so coroutine construction stays inside `boost::asio::co_spawn`.
template <class F>
inline void CoSpawn(AnyExecutor executor, F&& fn) {
  boost::asio::co_spawn(std::move(executor), std::forward<F>(fn),
                        boost::asio::detached);
}

template <class C, class F>
inline void CoSpawn(AnyExecutor executor,
                    std::weak_ptr<C> cancelation,
                    F&& fn) {
  using Fn = std::decay_t<F>;
  boost::asio::co_spawn(
      std::move(executor),
      [cancelation = std::move(cancelation),
       fn = Fn(std::forward<F>(fn))]() mutable -> Awaitable<void> {
        auto locked = cancelation.lock();
        if (!locked) {
          co_return;
        }

        if constexpr (std::is_invocable_v<Fn, std::shared_ptr<C>>) {
          co_await std::invoke(std::move(fn), std::move(locked));
        } else {
          static_assert(std::is_invocable_v<Fn>,
                        "CoSpawn(cancelation, fn) requires a nullary "
                        "coroutine factory or one that accepts shared_ptr<C>.");
          co_await std::invoke(std::move(fn));
        }
      },
      boost::asio::detached);
}

template <class F>
inline void CoSpawn(AnyExecutor executor,
                    const Cancelation& cancelation,
                    F&& fn) {
  CoSpawn(std::move(executor), cancelation.weak_ptr(), std::forward<F>(fn));
}

template <class ExecutionContext, class F>
auto RunAwaitable(ExecutionContext& context, F&& fn) {
  context.restart();
  auto result = boost::asio::co_spawn(context, std::forward<F>(fn),
                                      boost::asio::use_future);
  while (result.wait_for(std::chrono::seconds{0}) !=
         std::future_status::ready) {
    // The context can be stopped out from under us while the awaited
    // coroutine is still suspended, because the coroutine itself can ask the
    // process to stop: a module that rejects startup (e.g. license) calls
    // ServerProcess::Stop(), which calls io_context::stop() from inside the
    // startup coroutine RunAwaitable is driving. A stopped context runs
    // nothing, so without the restart the remaining startup work never
    // resumes and this loop spins on `run_one()` returning 0 forever.
    //
    // This is an explicit stop, not lost work accounting -- asio's
    // outstanding-work count is accurate here. ServerProcess::Run() re-reads
    // the intent from its own `stop_requested_` flag precisely because this
    // restart clears io_context::stopped().
    if (context.stopped())
      context.restart();
    context.run_one();
  }
  return result.get();
}
