#pragma once

#include <base/location.h>
#include <boost/assert/source_location.hpp>

namespace base {

inline Location ToLocation(const boost::source_location& location) {
  return Location{location.function_name(), location.file_name(),
                  static_cast<int>(location.line()), GetProgramCounter()};
}

}  // namespace base
