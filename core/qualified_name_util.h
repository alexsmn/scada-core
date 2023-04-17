#pragma once

#include "core/qualified_name.h"

#include <string_view>

namespace scada {

QualifiedName ToQualifiedName(std::u16string_view string);

}  // namespace scada
