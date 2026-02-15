#pragma once

#include "base/boost_log.h"

#include <base/strings/stringprintf.h>
#include <memory>
#include <transport/log.h>

class NetBoostLoggerAdapter final : public transport::LogSink {
 public:
  explicit NetBoostLoggerAdapter(std::shared_ptr<BoostLogger> boost_logger)
      : boost_logger_{std::move(boost_logger)} {}

  virtual void Write(transport::LogSeverity severity,
                     const char* message) const override {
    BOOST_LOG_SEV(*boost_logger_, ToBoostLogSeverity(severity)) << message;
  }

  virtual void WriteV(transport::LogSeverity severity,
                      const char* format,
                      va_list args) const override PRINTF_FORMAT(3, 0) {
    BOOST_LOG_SEV(*boost_logger_, ToBoostLogSeverity(severity))
        << base::StringPrintV(format, args);
  }

  virtual void WriteF(transport::LogSeverity severity,
                      const char* format,
                      ...) const override PRINTF_FORMAT(3, 4) {
    va_list args;
    va_start(args, format);
    auto message = base::StringPrintV(format, args);
    va_end(args);

    BOOST_LOG_SEV(*boost_logger_, ToBoostLogSeverity(severity)) << message;
  }

 private:
  static BoostLogSeverity ToBoostLogSeverity(transport::LogSeverity severity) {
    switch (severity) {
      case transport::LogSeverity::Normal:
        return BoostLogSeverity::info;
      case transport::LogSeverity::Warning:
        return BoostLogSeverity::warning;
      case transport::LogSeverity::Error:
        return BoostLogSeverity::error;
      case transport::LogSeverity::Fatal:
        return BoostLogSeverity::fatal;
      default:
        assert(false);
        return BoostLogSeverity::info;
    }
  }

  const std::shared_ptr<BoostLogger> boost_logger_;
};
