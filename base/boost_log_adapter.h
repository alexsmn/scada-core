#pragma once

#include "base/boost_log.h"
#include "base/logger.h"

class BoostLogAdapter : public Logger {
 public:
  explicit BoostLogAdapter(const std::string& name) : logger_{LOG_NAME(name)} {}

  void Write(LogSeverity severity, std::string_view message) const override {
    BOOST_LOG_SEV(logger_, ToBoostLogSeverity(severity)) << message;
  }

 private:
  static BoostLogSeverity ToBoostLogSeverity(LogSeverity severity) {
    switch (severity) {
      case LogSeverity::Normal:
        return BoostLogSeverity::info;
      case LogSeverity::Warning:
        return BoostLogSeverity::warning;
      case LogSeverity::Error:
        return BoostLogSeverity::error;
      case LogSeverity::Fatal:
        return BoostLogSeverity::fatal;
      default:
        assert(false);
        return BoostLogSeverity::info;
    }
  }

  mutable BoostLogger logger_;
};
