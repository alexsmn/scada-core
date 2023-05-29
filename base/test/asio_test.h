#pragma once

#include "base/asio_executor.h"
#include "base/promise.h"

#include <net/transport_factory_impl.h>

struct AsioTest {
  template <class T>
  T Wait(promise<T> promise) {
    using namespace std::chrono_literals;
    while (promise.wait_for(1ms) == promise_wait_status::timeout) {
      io_context_.run_for(100ms);
    }
    return promise.get();
  }

  void Wait(promise<> promise) {
    using namespace std::chrono_literals;
    while (promise.wait_for(1ms) == promise_wait_status::timeout) {
      io_context_.run_for(100ms);
    }
    promise.get();
  }

  void Poll() {
    while (io_context_.poll() != 0) {
    }
  }

  boost::asio::io_context io_context_;
  net::TransportFactoryImpl transport_factory_{io_context_};

  std::shared_ptr<AsioExecutor> executor_ =
      std::make_shared<AsioExecutor>(io_context_);
};
