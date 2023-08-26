#pragma once

#include <net/transport_interceptor.h>

class TestNetInterceptor : public net::TransportInterceptor {
 public:
  virtual std::optional<net::promise<size_t>> InterceptWrite(
      std::span<const char> data) const override {
    return swallow_writes
               ? std::optional<net::promise<size_t>>{net::make_resolved_promise(
                     data.size())}
               : std::nullopt;
  }

  bool swallow_writes = false;
};
