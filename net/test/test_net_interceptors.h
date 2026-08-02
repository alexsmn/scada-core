#pragma once

#include <transport/transport_interceptor.h>

namespace scada {

class TestNetInterceptor : public transport::TransportInterceptor {
 public:
  virtual std::optional<transport::expected<size_t>> InterceptWrite(
      std::span<const char> data) const override {
    return swallow_writes
               ? std::optional<transport::expected<size_t>>{data.size()}
               : std::nullopt;
  }

  bool swallow_writes = false;
};

}  // namespace scada

// Transitional compatibility shim: expose the historically global-scope name
// until all callers migrate to `scada::`.
using scada::TestNetInterceptor;  // NOLINT(build/namespaces) transitional
