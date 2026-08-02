#include "scada/range_encoding.h"

#include "scada/extension_object.h"

#include <any>

namespace scada {

ExtensionObject MakeRangeObject(Range range) {
  return ExtensionObject{ExpandedNodeId{NodeId{kRangeDataTypeId, 0}},
                         std::any{range}};
}

std::optional<Range> DecodeRangeObject(const ExtensionObject& object) {
  if (const auto* range = std::any_cast<Range>(&object.value())) {
    return *range;
  }
  return std::nullopt;
}

}  // namespace scada
