#include "scada/aggregate_filter.h"

#include "base/struct_writer.h"

#include "base/debug_util-inl.h"

namespace scada {

std::ostream& operator<<(std::ostream& stream, const AggregateFilter& filter) {
  StructWriter{stream}
      .AddField("start_time", filter.start_time)
      .AddField("interval", filter.interval)
      .AddField("aggregate_type", ToString(filter.aggregate_type));
  return stream;
}

}  // namespace scada
