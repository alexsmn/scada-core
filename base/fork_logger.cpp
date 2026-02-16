#include "base/fork_logger.h"

ForkLogger::~ForkLogger() {
  for (Loggers::iterator i = loggers_.begin(); i != loggers_.end(); ++i) {
    if (i->second)
      delete i->first;
  }
}

void ForkLogger::AddLogger(const Logger& logger, bool own) {
  loggers_.push_back(std::make_pair(&logger, own));
}

void ForkLogger::Write(LogSeverity severity, std::string_view message) const {
  for (auto& p : loggers_)
    p.first->Write(severity, message);
}
