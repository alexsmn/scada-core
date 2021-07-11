#pragma once

#include "base/struct_writer.h"
#include "core/date_time.h"
#include "core/node_id.h"

#include <ostream>

namespace scada {

struct AggregateFilter {
  bool is_null() const { return interval.is_zero(); }

  DateTime start_time;
  Duration interval;
  NodeId aggregate_type;
};

inline bool operator<(const AggregateFilter& a, const AggregateFilter& b) {
  return std::tie(a.start_time, a.interval, a.aggregate_type) <
         std::tie(b.start_time, b.interval, b.aggregate_type);
}

inline bool operator==(const AggregateFilter& a, const AggregateFilter& b) {
  return std::tie(a.start_time, a.interval, a.aggregate_type) ==
         std::tie(b.start_time, b.interval, b.aggregate_type);
}

inline std::ostream& operator<<(std::ostream& stream,
                                const AggregateFilter& filter) {
  StructWriter{stream}
      .AddField("start_time", filter.start_time)
      .AddField("interval", filter.interval)
      .AddField("aggregate_type", ToString(filter.aggregate_type));
  return stream;
}

}  // namespace scada
