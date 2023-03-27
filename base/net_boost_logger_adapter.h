#pragma once

#include "base/boost_log.h"

#include <base/strings/stringprintf.h>
#include <memory>
#include <net/logger.h>

class NetBoostLoggerAdapter final : public net::Logger {
 public:
  explicit NetBoostLoggerAdapter(std::shared_ptr<BoostLogger> boost_logger)
      : boost_logger_{std::move(boost_logger)} {}

  virtual void Write(net::LogSeverity severity,
                     const char* message) const override {
    BOOST_LOG_SEV(*boost_logger_, ToBoostLogSeverity(severity)) << message;
  }

  virtual void WriteV(net::LogSeverity severity,
                      const char* format,
                      va_list args) const override PRINTF_FORMAT(3, 0) {
    BOOST_LOG_SEV(*boost_logger_, ToBoostLogSeverity(severity))
        << base::StringPrintV(format, args);
  }

  virtual void WriteF(net::LogSeverity severity,
                      const char* format,
                      ...) const override PRINTF_FORMAT(3, 4) {
    va_list args;
    va_start(args, format);
    auto message = base::StringPrintV(format, args);
    va_end(args);

    BOOST_LOG_SEV(*boost_logger_, ToBoostLogSeverity(severity)) << message;
  }

 private:
  static BoostLogSeverity ToBoostLogSeverity(net::LogSeverity severity) {
    switch (severity) {
      case net::LogSeverity::Normal:
        return BoostLogSeverity::Info;
      case net::LogSeverity::Warning:
        return BoostLogSeverity::Warning;
      case net::LogSeverity::Error:
        return BoostLogSeverity::Error;
      case net::LogSeverity::Fatal:
        return BoostLogSeverity::Critical;
      default:
        assert(false);
        return BoostLogSeverity::Info;
    }
  }

  const std::shared_ptr<BoostLogger> boost_logger_;
};
