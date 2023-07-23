#pragma once

#include "base/containers/span.h"
#include "scada/variant.h"

#include <any>

namespace scada {

std::any AssembleEvent(base::span<const scada::Variant> fields);
std::vector<scada::Variant> DisassembleEvent(const std::any& event);

}  // namespace scada
