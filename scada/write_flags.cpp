#include "scada/write_flags.h"

#include "base/bit_mask_string.h"


namespace scada {

std::ostream& operator<<(std::ostream& stream, WriteFlags flags) {
  constexpr std::string_view kBitStrings[] = {
      "Select",
      "Parameter",
  };
  return stream << base::BitMaskToString(flags.raw(), kBitStrings);
}

}  // namespace scada
