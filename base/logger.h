#pragma once

#include <format>
#include <memory>
#include <string>
#include <string_view>

enum class LogSeverity {
  Normal = 0,
  Warning = 1,
  Error = 2,
  Fatal = 3,
  Count = 4
};

class Logger {
 public:
  virtual ~Logger() = default;

  virtual void Write(LogSeverity severity, std::string_view message) const = 0;

  template <typename... Args>
  void WriteF(LogSeverity severity,
              std::format_string<Args...> fmt,
              Args&&... args) const {
    Write(severity, std::format(fmt, std::forward<Args>(args)...));
  }
};

class NullLogger : public Logger {
 public:
  void Write(LogSeverity severity, std::string_view message) const override {}

  static std::shared_ptr<NullLogger> GetInstance() {
    static auto logger = std::make_shared<NullLogger>();
    return logger;
  }
};
