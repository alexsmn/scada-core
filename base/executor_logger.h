#pragma once

#include "base/executor.h"
#include "base/logger.h"
#include "base/strings/stringprintf.h"

class ExecutorLogger final : public Logger {
 public:
  ExecutorLogger(std::shared_ptr<const Logger> logger,
                 std::shared_ptr<Executor> executor)
      : logger_{std::move(logger)}, executor_{executor} {}

  virtual void Write(LogSeverity severity, const char* message) const override {
    Dispatch(*executor_, [logger = logger_, severity,
                          copied_message = std::string{message}] {
      logger->Write(severity, copied_message.c_str());
    });
  }

  virtual void WriteV(LogSeverity severity,
                      const char* format,
                      va_list args) const override {
    Dispatch(*executor_,
             [logger = logger_, severity,
              formatted_message = base::StringPrintV(format, args)] {
               logger->Write(severity, formatted_message.c_str());
             });
  }

  virtual void WriteF(LogSeverity severity,
                      const char* format,
                      ...) const override {
    va_list args;
    va_start(args, format);
    Dispatch(*executor_,
             [logger = logger_, severity,
              formatted_message = base::StringPrintV(format, args)] {
               logger->Write(severity, formatted_message.c_str());
             });
    va_end(args);
  }

 private:
  const std::shared_ptr<const Logger> logger_;
  const std::shared_ptr<Executor> executor_;
};
