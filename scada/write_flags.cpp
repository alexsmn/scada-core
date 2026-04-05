#include "scada/write_flags.h"

#include "base/debug_util.h"


namespace scada {

std::ostream& operator<<(std::ostream& stream, WriteFlags flags) {
  constexpr std::string_view kBitStrings[] = {
      "Select",
      "Parameter",
  };
  return stream << BitMaskToString(flags.raw(), kBitStrings);
}

}  // namespace scada
