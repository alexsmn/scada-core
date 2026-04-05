#pragma once

#include "base/boost_log.h"

#include <memory>
#include <string_view>
#include <transport/log.h>

class NetBoostLoggerAdapter final : public transport::LogSink {
 public:
  explicit NetBoostLoggerAdapter(std::shared_ptr<BoostLogger> boost_logger)
      : boost_logger_{std::move(boost_logger)} {}

  void Write(transport::LogSeverity severity,
             std::string_view message) const override {
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
