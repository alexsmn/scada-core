#include "base/console_logger.h"

#include <boost/log/trivial.hpp>

#include <iostream>

namespace {

const char kSeverityNames[] = "NWEF";
static_assert(std::size(kSeverityNames) ==
                  static_cast<size_t>(LogSeverity::Count) + 1,
              "Not all severities are described");

#ifdef OS_WIN
class ScopedConsoleAttributes {
 public:
  ScopedConsoleAttributes(HANDLE console,
                          WORD initial_attributes,
                          LogSeverity severity)
      : console_(console),
        initial_attributes_(initial_attributes),
        attributes_(initial_attributes) {
    if (severity >= LogSeverity::Error)
      attributes_ = FOREGROUND_RED | FOREGROUND_INTENSITY;
    else if (severity >= LogSeverity::Warning)
      attributes_ = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE |
                    FOREGROUND_INTENSITY;
    else
      attributes_ = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;

    if (attributes_ != initial_attributes_)
      SetConsoleTextAttribute(console_, attributes_);
  }

  ~ScopedConsoleAttributes() {
    if (attributes_ != initial_attributes_)
      SetConsoleTextAttribute(console_, initial_attributes_);
  }

 private:
  HANDLE console_;
  WORD initial_attributes_;
  WORD attributes_;
};
#endif

}  // namespace

#ifdef OS_WIN
ConsoleLogger::ConsoleLogger()
    : console_(GetStdHandle(STD_OUTPUT_HANDLE)), initial_attributes_(0) {
  CONSOLE_SCREEN_BUFFER_INFO info;
  if (GetConsoleScreenBufferInfo(console_, &info))
    initial_attributes_ = info.wAttributes;
}
#else
ConsoleLogger::ConsoleLogger() {}
#endif

void ConsoleLogger::Write(LogSeverity severity,
                          std::string_view message) const {
#ifdef OS_WIN
  ScopedConsoleAttributes scoped_attrs(console_, initial_attributes_, severity);
#endif
  std::cout << kSeverityNames[static_cast<size_t>(severity)] << " " << message
            << std::endl;
  BOOST_LOG_TRIVIAL(info) << kSeverityNames[static_cast<size_t>(severity)] << " " << message;
}
