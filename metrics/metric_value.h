#pragma once

#include <variant>

using MetricValue = std::variant<std::int64_t, double>;
