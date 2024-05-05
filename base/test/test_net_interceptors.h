#pragma once

#include <net/transport_interceptor.h>

class TestNetInterceptor : public net::TransportInterceptor {
 public:
  virtual std::optional<net::ErrorOr<size_t>> InterceptWrite(
      std::span<const char> data) const override {
    return swallow_writes ? std::optional<net::ErrorOr<size_t>>{data.size()}
                          : std::nullopt;
  }

  bool swallow_writes = false;
};
