#pragma once

#ifdef OS_WIN
#include <windows.h>
#endif

#include "base/logger.h"

class ConsoleLogger : public Logger {
 public:
  ConsoleLogger();

  // Logger
  virtual void Write(LogSeverity severity, const char* message) const override;
  virtual void WriteV(LogSeverity severity,
                      _Printf_format_string_ const char* format,
                      va_list args) const override PRINTF_FORMAT(3, 0);
  virtual void WriteF(LogSeverity severity,
                      _Printf_format_string_ const char* format,
                      ...) const override PRINTF_FORMAT(3, 4);

  static const ConsoleLogger& GetInstance() {
    static const ConsoleLogger s_logger;
    return s_logger;
  }

 private:
#ifdef OS_WIN
  HANDLE console_;
  WORD initial_attributes_;
#endif
};
