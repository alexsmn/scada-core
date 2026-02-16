#pragma once

#include "base/logger.h"

#include <memory>
#include <string>

class NestedLogger : public Logger {
 public:
  NestedLogger();
  NestedLogger(std::shared_ptr<const Logger> parent, const std::string& prefix);

  const Logger* parent() const { return parent_.get(); }
  void set_parent(std::shared_ptr<const Logger> parent) {
    parent_ = std::move(parent);
  }

  void set_prefix(const std::string& prefix);

  // Logger overrides
  void Write(LogSeverity severity, std::string_view message) const override;

 private:
  std::shared_ptr<const Logger> parent_;
  std::string prefix_;
};
