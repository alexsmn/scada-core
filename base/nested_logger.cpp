#include "base/nested_logger.h"

#include <cassert>
#include <string>

NestedLogger::NestedLogger() {}

NestedLogger::NestedLogger(std::shared_ptr<const Logger> parent,
                           const std::string& prefix)
    : parent_(parent) {
  set_prefix(prefix);
}

void NestedLogger::set_prefix(const std::string& prefix) {
  if (!prefix.empty())
    prefix_ = prefix + ": ";
  else
    prefix_.clear();
}

void NestedLogger::Write(LogSeverity severity,
                         std::string_view message) const {
  if (parent_)
    parent_->Write(severity, prefix_ + std::string(message));
}
