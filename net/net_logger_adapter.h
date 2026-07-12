#pragma once

#include "base/logger.h"

#include <memory>
#include <transport/log.h>

namespace scada {

class NetLoggerAdapter final : public transport::LogSink {
 public:
  explicit NetLoggerAdapter(std::shared_ptr<const ::Logger> logger)
      : logger_{std::move(logger)} {}

  void Write(transport::LogSeverity severity,
             std::string_view message) const override {
    logger_->Write(static_cast<::LogSeverity>(severity), message);
  }

 private:
  const std::shared_ptr<const ::Logger> logger_;
};

inline std::shared_ptr<transport::LogSink> CreateNetLoggerAdapter(
    std::shared_ptr<const ::Logger> logger) {
  return logger ? std::make_shared<NetLoggerAdapter>(std::move(logger))
                : nullptr;
}

}  // namespace scada

// Transitional compatibility shim: expose the historically global-scope names
// until all callers migrate to `scada::`.
using scada::CreateNetLoggerAdapter;  // NOLINT(build/namespaces) transitional
using scada::NetLoggerAdapter;        // NOLINT(build/namespaces) transitional
