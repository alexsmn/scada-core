#pragma once

#include "base/executor.h"
#include "base/logger.h"

class ExecutorLogger final : public Logger {
 public:
  ExecutorLogger(std::shared_ptr<const Logger> logger,
                 std::shared_ptr<Executor> executor)
      : logger_{std::move(logger)}, executor_{executor} {}

  void Write(LogSeverity severity, std::string_view message) const override {
    Dispatch(*executor_, [logger = logger_, severity,
                          copied_message = std::string{message}] {
      logger->Write(severity, copied_message);
    });
  }

 private:
  const std::shared_ptr<const Logger> logger_;
  const std::shared_ptr<Executor> executor_;
};
