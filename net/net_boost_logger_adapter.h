#pragma once

#include "base/boost_log.h"
#include "base/check.h"

#include <memory>
#include <string_view>
#include <transport/log.h>

namespace scada {

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
        base::NotReached();
        return BoostLogSeverity::info;
    }
  }

  const std::shared_ptr<BoostLogger> boost_logger_;
};

}  // namespace scada

// Transitional compatibility shim: expose the historically global-scope name
// until all callers migrate to `scada::`.
using scada::NetBoostLoggerAdapter;  // NOLINT(build/namespaces) transitional
