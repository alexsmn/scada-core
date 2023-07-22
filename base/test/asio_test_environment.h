#pragma once

#include "base/asio_executor.h"
#include "base/promise.h"

#include <net/transport_factory_impl.h>

struct AsioTestEnvironment {
  template <class T>
  T Wait(promise<T> promise) {
    using namespace std::chrono_literals;
    while (promise.wait_for(1ms) == promise_wait_status::timeout) {
      io_context.run_for(100ms);
    }
    return promise.get();
  }

  void Wait(promise<> promise) {
    using namespace std::chrono_literals;
    while (promise.wait_for(1ms) == promise_wait_status::timeout) {
      io_context.run_for(100ms);
    }
    promise.get();
  }

  void Poll() {
    while (io_context.poll() != 0) {
    }
  }

  boost::asio::io_context io_context;
  net::TransportFactoryImpl transport_factory{io_context};

  const std::shared_ptr<AsioExecutor> executor =
      std::make_shared<AsioExecutor>(io_context);
};
