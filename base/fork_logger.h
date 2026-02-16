#pragma once

#include "base/logger.h"

#include <vector>

class ForkLogger : public Logger {
 public:
  virtual ~ForkLogger();
 
  void AddLogger(const Logger& logger, bool own);
 
  // Logger
  void Write(LogSeverity severity, std::string_view message) const override;
  
 private:
  typedef std::vector<std::pair<const Logger*, bool> > Loggers;
  Loggers loggers_;
};
