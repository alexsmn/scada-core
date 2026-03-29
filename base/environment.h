#pragma once

#include <cstdlib>
#include <memory>
#include <string>
#include <string_view>

#ifdef _WIN32
#include <windows.h>
#endif

namespace base {

class Environment {
 public:
  static std::unique_ptr<Environment> Create() {
    return std::make_unique<Environment>();
  }

  bool GetVar(std::string_view name, std::string* result) const {
#ifdef _WIN32
    // Windows: use GetEnvironmentVariableA for proper handling.
    std::string name_str{name};
    DWORD size = GetEnvironmentVariableA(name_str.c_str(), nullptr, 0);
    if (size == 0)
      return false;
    result->resize(size - 1);
    GetEnvironmentVariableA(name_str.c_str(), result->data(), size);
    return true;
#else
    const char* val = std::getenv(std::string{name}.c_str());
    if (!val)
      return false;
    *result = val;
    return true;
#endif
  }
};

}  // namespace base
