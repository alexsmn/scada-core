#pragma once

#include "base/struct_writer.h"
#include "scada/date_time.h"
#include "scada/node_id.h"

#include <ostream>

namespace scada {

struct AggregateFilter {
  bool is_null() const { return interval.is_zero(); }

  auto operator<=>(const AggregateFilter&) const = default;

  DateTime start_time;
  Duration interval;
  NodeId aggregate_type;
};

inline std::ostream& operator<<(std::ostream& stream,
                                const AggregateFilter& filter) {
  StructWriter{stream}
      .AddField("start_time", filter.start_time)
      .AddField("interval", filter.interval)
      .AddField("aggregate_type", ToString(filter.aggregate_type));
  return stream;
}

}  // namespace scada
