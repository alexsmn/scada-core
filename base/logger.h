#pragma once

#include "base/compiler_specific.h"

#include <cstdarg>
#include <memory>
#include <string>

enum class LogSeverity {
  Normal = 0,
  Warning = 1,
  Error = 2,
  Fatal = 3,
  Count = 4
};

class Logger {
 public:
  virtual ~Logger() {}

  virtual void Write(LogSeverity severity, const char* message) const = 0;
  virtual void WriteV(LogSeverity severity,
                      _Printf_format_string_ const char* format,
                      va_list args) const PRINTF_FORMAT(3, 0) = 0;
  virtual void WriteF(LogSeverity severity,
                      _Printf_format_string_ const char* format,
                      ...) const PRINTF_FORMAT(3, 4) = 0;
};

class NullLogger : public Logger {
 public:
  virtual void Write(LogSeverity severity, const char* message) const override {
  }
  virtual void WriteV(LogSeverity severity,
                      _Printf_format_string_ const char* format,
                      va_list args) const override PRINTF_FORMAT(3, 0) {}
  virtual void WriteF(LogSeverity severity,
                      _Printf_format_string_ const char* format,
                      ...) const override PRINTF_FORMAT(3, 4) {}

  static std::shared_ptr<NullLogger> GetInstance() {
    static auto logger = std::make_shared<NullLogger>();
    return logger;
  }
};
