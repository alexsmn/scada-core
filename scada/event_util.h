#pragma once

#include "scada/variant.h"

#include <any>
#include <span>

namespace scada {

std::any AssembleEvent(std::span<const scada::Variant> fields);
std::vector<scada::Variant> DisassembleEvent(const std::any& event);

}  // namespace scada
