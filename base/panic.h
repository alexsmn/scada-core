#pragma once

#include <string_view>

namespace base {

[[noreturn]] void Panic(std::string_view message);

}  // namespace base
