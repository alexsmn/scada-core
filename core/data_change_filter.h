#pragma once

#include "core/basic_types.h"

namespace scada {

struct DataChangeFilter {
  // Deadband type: Absolute, Percent.
  Double deadband_value;
};

}  // namespace scada
