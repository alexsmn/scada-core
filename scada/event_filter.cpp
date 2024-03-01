#include "scada/event_filter.h"

#include "base/struct_writer.h"

#include "base/debug_util-inl.h"

namespace scada {

std::ostream& operator<<(std::ostream& stream,
                         const EventFilter& event_filter) {
  constexpr std::string_view kTypeBitStrings[] = {"ACKED", "UNACKED"};

  StructWriter{stream}
      .AddBitMaskField("types", event_filter.types, kTypeBitStrings)
      .AddField("of_type", event_filter.of_type)
      .AddField("child_of", event_filter.child_of);

  return stream;
}

}  // namespace scada