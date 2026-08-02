#pragma once

#include "scada/node_id.h"

#include <optional>

namespace scada {

class ExtensionObject;

// OPC UA DataType NodeId of the Range structure (ns=0, i=884) and its
// DefaultBinary encoding object (i=886), verified against the official 1.05
// NodeIds.csv (https://files.opcfoundation.org/schemas/UA/1.05/NodeIds.csv).
// OPC UA Part 8 §5.6.2 Range,
// https://reference.opcfoundation.org/Core/Part8/v105/docs/5.6.2
inline constexpr NumericId kRangeDataTypeId = 884;
inline constexpr NumericId kRangeDefaultBinaryId = 886;

// An OPC UA Range: an inclusive low/high pair of Doubles. Used for the
// UserManagement object's PasswordLength property (OPC UA Part 18 §5.2.2),
// where Low is the minimum and High the maximum password length.
struct Range {
  double low = 0;
  double high = 0;

  friend bool operator==(const Range&, const Range&) = default;
};

// Wraps a range as an ExtensionObject carrying the Range DataType id.
ExtensionObject MakeRangeObject(Range range);

// Extracts the range from an ExtensionObject payload, or nullopt when the
// payload is not a Range.
std::optional<Range> DecodeRangeObject(const ExtensionObject& object);

}  // namespace scada
