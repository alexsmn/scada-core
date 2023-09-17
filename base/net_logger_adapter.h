#pragma once

#include "base/logger.h"

#include <memory>
#include <net/logger.h>

class NetLoggerAdapter final : public net::Logger {
 public:
  explicit NetLoggerAdapter(std::shared_ptr<const ::Logger> logger)
      : logger_{std::move(logger)} {}

  virtual void Write(net::LogSeverity severity,
                     const char* message) const override {
    logger_->Write(static_cast<::LogSeverity>(severity), message);
  }

  virtual void WriteV(net::LogSeverity severity,
                      const char* format,
                      va_list args) const override PRINTF_FORMAT(3, 0) {
    logger_->WriteV(static_cast<::LogSeverity>(severity), format, args);
  }

  virtual void WriteF(net::LogSeverity severity,
                      _Printf_format_string_ const char* format,
                      ...) const override PRINTF_FORMAT(3, 4) {
    va_list args;
    va_start(args, format);
    logger_->WriteV(static_cast<::LogSeverity>(severity), format, args);
    va_end(args);
  }

 private:
  const std::shared_ptr<const ::Logger> logger_;
};

inline std::shared_ptr<net::Logger> CreateNetLoggerAdapter(
    std::shared_ptr<const ::Logger> logger) {
  return logger ? std::make_shared<NetLoggerAdapter>(std::move(logger))
                : nullptr;
}
