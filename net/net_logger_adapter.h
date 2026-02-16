#pragma once

#include "base/logger.h"

#include <memory>
#include <transport/log.h>

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
