#pragma once

#include "scada/date_time.h"
#include "scada/node_id.h"

#include <concepts>
#include <ostream>

namespace scada {

struct AggregateFilter {
  bool is_null() const { return interval.is_zero(); }

  std::strong_ordering operator<=>(const AggregateFilter&) const = default;

  DateTime start_time;
  Duration interval;
  NodeId aggregate_type;
};

static_assert(std::totally_ordered<AggregateFilter>);

std::ostream& operator<<(std::ostream& stream, const AggregateFilter& filter);

}  // namespace scada
