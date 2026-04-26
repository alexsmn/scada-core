#pragma once

#include "base/awaitable.h"
#include "base/awaitable_promise.h"
#include "base/asio_executor.h"
#include "base/executor_adapter.h"
#include "base/executor_factory.h"
#include "base/promise.h"
#include "net/test/test_net_interceptors.h"

#include <boost/asio/any_io_executor.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/strand.hpp>
#include <transport/intercepting_transport_factory.h>
#include <transport/transport_factory_impl.h>

#include <chrono>

struct AsioTestEnvironment {
  AsioTestEnvironment() {
    transport_factory.set_interceptor(&transport_interceptor);
  }

  template <class T>
  T Wait(promise<T> promise) {
    using namespace std::chrono_literals;
    while (promise.wait_for(0ms) == promise_wait_status::timeout) {
      RunOneReadyOrBlockFor(1ms);
    }
    Poll();
    return promise.get();
  }

  void Wait(promise<> promise) {
    using namespace std::chrono_literals;
    while (promise.wait_for(0ms) == promise_wait_status::timeout) {
      RunOneReadyOrBlockFor(1ms);
    }
    Poll();
    promise.get();
  }

  template <class T>
  T Wait(Awaitable<T> awaitable) {
    return Wait(ToPromise(any_executor_factory(), std::move(awaitable)));
  }

  void Wait(Awaitable<void> awaitable) {
    Wait(ToPromise(any_executor_factory(), std::move(awaitable)));
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

  boost::asio::io_context io_context;
  // Work guard to prevent io_context from stopping when there are no more
  // tasks.
  boost::asio::executor_work_guard<boost::asio::io_context::executor_type>
      work = boost::asio::make_work_guard(io_context);

  transport::TransportFactoryImpl transport_factory_impl;
  TestNetInterceptor transport_interceptor;
  transport::InterceptingTransportFactory transport_factory{
      transport_factory_impl};

  const std::shared_ptr<Executor> executor = std::make_shared<AsioExecutor>(
      boost::asio::make_strand(io_context.get_executor()));

  const ExecutorFactory executor_factory = MakeSingleExecutorFactory(executor);

  const AnyExecutorFactory any_executor_factory = [this]() -> AnyExecutor {
    return AnyExecutor{ExecutorAdapter{executor}};
  };
};
