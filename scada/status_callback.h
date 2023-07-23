#pragma once

#include "scada/status.h"

#include <functional>

namespace scada {

using StatusCallback = std::function<void(Status&&)>;
using MultiStatusCallback =
    std::function<void(Status&&, std::vector<StatusCode>&&)>;

}  // namespace scada
