#pragma once

#include "base/awaitable.h"
#include "base/any_executor.h"
#include "net/test/test_net_interceptors.h"

#include <boost/asio/co_spawn.hpp>
#include <boost/asio/any_io_executor.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/use_future.hpp>
#include <boost/asio/strand.hpp>
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
        [result, awaitable = std::move(awaitable)]() mutable
            -> Awaitable<void> {
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

  template <class T>
  T Wait(Awaitable<T> awaitable) {
    return WaitResult(Start(std::move(awaitable)));
  }

  void Wait(Awaitable<void> awaitable) {
    WaitResult(Start(std::move(awaitable)));
  }

  void Poll() {
    io_context.restart();
    while (io_context.poll() != 0) {
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

  void PumpFor(std::chrono::milliseconds duration,
               std::chrono::milliseconds step = std::chrono::milliseconds{1}) {
    const auto deadline = std::chrono::steady_clock::now() + duration;
    while (std::chrono::steady_clock::now() < deadline) {
      RunOneReadyOrBlockFor(step);
    }
    Poll();
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

  const AnyExecutor executor = boost::asio::make_strand(io_context.get_executor());

  const AnyExecutorFactory executor_factory = MakeSingleExecutorFactory(executor);

  const AnyExecutorFactory any_executor_factory = executor_factory;
};
