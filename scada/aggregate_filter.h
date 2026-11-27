#pragma once

#include "scada/date_time.h"
#include "scada/node_id.h"

#include <concepts>
#include <ostream>

namespace scada {

// The longest processing interval a request may carry. Far beyond any real
// aggregate, and far enough from the Duration maximum that bucket arithmetic
// (start + k * interval + interval) cannot overflow.
inline constexpr Duration kMaxAggregateInterval =
    std::chrono::hours{24 * 366 * 100};

// An aggregate request: `aggregate_type` computed over consecutive buckets of
// `interval` from `start_time`. A zero interval means "no aggregate" (raw).
struct AggregateFilter {
  bool is_null() const { return interval == Duration::zero(); }

  // True when a non-null filter can be computed: an aggregate type and a
  // positive interval no longer than kMaxAggregateInterval. The fields come
  // from clients, and a negative interval inverts every bucket, which the
  // aggregation code asserts never happens.
  bool is_valid() const {
    return !aggregate_type.is_null() && interval > Duration::zero() &&
           interval <= kMaxAggregateInterval;
  }

  std::strong_ordering operator<=>(const AggregateFilter&) const = default;

  Time start_time = scada::kNullTime;
  Duration interval = Duration::zero();
  NodeId aggregate_type;
};

static_assert(std::totally_ordered<AggregateFilter>);

std::ostream& operator<<(std::ostream& stream, const AggregateFilter& filter);

}  // namespace scada
