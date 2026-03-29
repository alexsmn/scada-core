#pragma once

#ifdef _WIN32
#include <windows.h>
#endif

#include "base/logger.h"

class ConsoleLogger : public Logger {
 public:
  ConsoleLogger();

  // Logger
  void Write(LogSeverity severity, std::string_view message) const override;

  static const ConsoleLogger& GetInstance() {
    static const ConsoleLogger s_logger;
    return s_logger;
  }

 private:
#ifdef _WIN32
  HANDLE console_;
  WORD initial_attributes_;
#endif
};
