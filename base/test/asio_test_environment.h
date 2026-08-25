#pragma once

#include "base/any_executor.h"
#include "base/awaitable.h"
#include "net/test/test_net_interceptors.h"

#include <boost/asio/any_io_executor.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/use_future.hpp>
#include <transport/intercepting_transport_factory.h>
#include <transport/transport_factory_impl.h>

#include <chrono>
#include <exception>
#include <future>
#include <optional>
#include <type_traits>

template <class T>
struct AsioAwaitableResult {
  std::optional<T> value;
  std::exception_ptr error;
  bool done = false;
};

template <>
struct AsioAwaitableResult<void> {
  std::exception_ptr error;
  bool done = false;
};

struct AsioTestEnvironment {
  AsioTestEnvironment() {
    transport_factory.set_interceptor(&transport_interceptor);
  }

  template <class T>
  std::shared_ptr<AsioAwaitableResult<T>> Start(Awaitable<T> awaitable) {
    auto result = std::make_shared<AsioAwaitableResult<T>>();
    boost::asio::co_spawn(
        executor,
        [result,
         awaitable = std::move(awaitable)]() mutable -> Awaitable<void> {
          try {
            if constexpr (std::is_void_v<T>) {
              co_await std::move(awaitable);
            } else {
              result->value.emplace(co_await std::move(awaitable));
            }
          } catch (...) {
            result->error = std::current_exception();
          }
          result->done = true;
        },
        boost::asio::detached);
    return result;
  }

  template <class T>
  T WaitResult(std::shared_ptr<AsioAwaitableResult<T>> result) {
    using namespace std::chrono_literals;
    while (!result->done) {
      RunOneReadyOrBlockFor(1ms);
    }
    Poll();
    if (result->error) {
      std::rethrow_exception(result->error);
    }
    return std::move(*result->value);
  }

  void WaitResult(std::shared_ptr<AsioAwaitableResult<void>> result) {
    using namespace std::chrono_literals;
    while (!result->done) {
      RunOneReadyOrBlockFor(1ms);
    }
    Poll();
    if (result->error) {
      std::rethrow_exception(result->error);
    }
  }

  // Pumps the io_context until `result` completes or `timeout` elapses.
  // Returns true on completion (rethrowing a stored error); false on timeout,
  // leaving the awaitable pending — it may still complete on later pumps.
  template <class T>
  bool WaitResultFor(const std::shared_ptr<AsioAwaitableResult<T>>& result,
                     std::chrono::steady_clock::duration timeout) {
    using namespace std::chrono_literals;
    const auto deadline = std::chrono::steady_clock::now() + timeout;
    while (!result->done && std::chrono::steady_clock::now() < deadline) {
      RunOneReadyOrBlockFor(1ms);
    }
    if (!result->done) {
      return false;
    }
    Poll();
    if (result->error) {
      std::rethrow_exception(result->error);
    }
    return true;
  }

  template <class T>
  T Wait(Awaitable<T> awaitable) {
    return WaitResult(Start(std::move(awaitable)));
  }

  void Wait(Awaitable<void> awaitable) {
    WaitResult(Start(std::move(awaitable)));
  }

  // Drains every handler that is ready now, plus any that become ready while
  // draining — but for at most one second. The time budget guards against
  // self-sustaining io_context work: two in-process device peers polling each
  // other over loopback TCP can keep the ready queue non-empty on every pass
  // (each response completes a read that immediately issues the next
  // request), which turned this drain into an infinite 98%-CPU spin inside
  // test waits whenever the machine was loaded enough that a new completion
  // always arrived before poll() ran dry.
  void Poll() {
    const auto deadline =
        std::chrono::steady_clock::now() + std::chrono::seconds{1};
    io_context.restart();
    while (io_context.poll() != 0 &&
           std::chrono::steady_clock::now() < deadline) {
    }
  }

  template <class Rep, class Period>
  void RunOneReadyOrBlockFor(
      const std::chrono::duration<Rep, Period>& timeout) {
    io_context.restart();
    if (io_context.poll_one() == 0) {
      io_context.restart();
      io_context.run_one_for(timeout);
    }
  }

  // Waits out `duration` in full, whatever happens. Prefer DrainUntil or
  // RunUntil below: both end as soon as the thing being waited for has
  // happened, so a test pays what the work costs rather than the budget its
  // author guessed. A fixed span is still right when the wait is on something
  // no predicate can observe -- a real timer deliberately NOT firing, for
  // instance -- in which case say so in a comment at the call site.
  void PumpFor(std::chrono::milliseconds duration,
               std::chrono::milliseconds step = std::chrono::milliseconds{1}) {
    const auto deadline = std::chrono::steady_clock::now() + duration;
    while (std::chrono::steady_clock::now() < deadline) {
      RunOneReadyOrBlockFor(step);
    }
    Poll();
  }

  // Drains ready handlers until `predicate` holds, then drains once more so
  // the handlers the satisfying one posted also run. Returns false if
  // `max_iterations` passes go by without it holding, so a caller can assert
  // the wait actually succeeded instead of proceeding on a silent timeout.
  //
  // Only for work that is handler-driven end to end: Poll() never blocks, so
  // a wait that depends on the reactor -- socket readiness above all -- spins
  // past it and times out. Use RunUntil for those.
  template <class Predicate>
  bool DrainUntil(Predicate&& predicate, int max_iterations = 1000) {
    for (int i = 0; i < max_iterations && !predicate(); ++i) {
      Poll();
    }
    if (!predicate()) {
      return false;
    }
    Poll();
    return true;
  }

  // As DrainUntil, but blocks for up to `step` whenever nothing is ready, so
  // it also waits on reactor events. This is the general replacement for a
  // fixed PumpFor whose wait has an observable outcome.
  template <class Predicate, class Rep, class Period>
  bool RunUntil(Predicate&& predicate,
                const std::chrono::duration<Rep, Period>& step,
                int max_iterations = 1000) {
    for (int i = 0; i < max_iterations && !predicate(); ++i) {
      RunOneReadyOrBlockFor(step);
    }
    if (!predicate()) {
      return false;
    }
    Poll();
    return true;
  }

  boost::asio::io_context io_context;
  // Work guard to prevent io_context from stopping when there are no more
  // tasks.
  boost::asio::executor_work_guard<boost::asio::io_context::executor_type>
      work = boost::asio::make_work_guard(io_context);

  transport::TransportFactoryImpl transport_factory_impl;
  TestNetInterceptor transport_interceptor;
  transport::InterceptingTransportFactory transport_factory{
      transport_factory_impl};

  const AnyExecutor executor =
      boost::asio::make_strand(io_context.get_executor());

  const AnyExecutorFactory executor_factory =
      MakeSingleExecutorFactory(executor);

  const AnyExecutorFactory any_executor_factory = executor_factory;
};
