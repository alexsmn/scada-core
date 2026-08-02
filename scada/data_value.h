#pragma once

#include "base/check.h"
#include "scada/date_time.h"
#include "scada/qualifier.h"
#include "scada/variant.h"

namespace scada {

class DataValue {
 public:
  constexpr DataValue() = default;

  constexpr DataValue(StatusCode status_code, Time server_timestamp)
      : server_timestamp{server_timestamp}, status_code{status_code} {
    base::Check(!IsGood(status_code));
  }

  template <class T>
  DataValue(T&& value,
            Qualifier qualifier,
            Time source_timestamp,
            Time server_timestamp)
      : value(std::forward<T>(value)),
        qualifier(std::move(qualifier)),
        source_timestamp(source_timestamp),
        server_timestamp(server_timestamp) {
    if (qualifier.failed())
      status_code = StatusCode::Bad;
  }

  DataValue(const DataValue&) = default;
  DataValue& operator=(const DataValue&) = default;

  DataValue(DataValue&&) = default;
  DataValue& operator=(DataValue&&) = default;

  constexpr bool is_null() const noexcept {
    return value.is_null() && qualifier.raw() == 0;
  }

  bool operator==(const DataValue& other) const {
    return source_timestamp == other.source_timestamp &&
           server_timestamp == other.server_timestamp && value == other.value &&
           qualifier == other.qualifier && status_code == other.status_code;
  }

  Variant value;
  Qualifier qualifier;
  Time source_timestamp = scada::kNullTime;
  Time server_timestamp = scada::kNullTime;
  StatusCode status_code = StatusCode::Good;
};

// The StatusCode a DataValue must be reported with when the server publishes it
// to a client (a Read result or a data-change Notification).
//
// A DataValue whose Variant is empty carries no value at all — no datatype, no
// contents. Reporting that at Good severity tells the client the result "may be
// used" (OPC UA Part 4 §7.38 StatusCode,
// https://reference.opcfoundation.org/Core/Part4/v105/docs/7.38) when there is
// nothing to use, and it inverts the Data Access pairing rule that "the Server
// shall return a Null value when the Severity is Bad" (OPC UA Part 8 §7.3 Data
// Access status codes,
// https://reference.opcfoundation.org/Core/Part8/v105/docs/7.3). The spec code
// for exactly this state is Bad_WaitingForInitialData (OPC UA Part 4 §7.38.2
// Common StatusCodes,
// https://reference.opcfoundation.org/Core/Part4/v105/docs/7.38.2).
//
// A present value is reported with its Qualifier projected onto a StatusCode by
// `Qualifier::ToStatus()`. Without that step the SCADA quality model never
// reaches a standard OPC UA client at all: `Qualifier` travels as an opcuapp
// extension field that only a SCADA peer decodes, so a stale or disconnected
// reading would otherwise be published at Good severity — the same "asserting
// Good over something unusable" defect as the empty-Variant case, one step
// further along.
//
// Precedence, highest first:
//   1. A non-Good `status_code` — it names a specific reason the producer
//      already determined (Bad_WrongNodeId, a device fault, ...) and must not
//      be softened by a quality projection.
//   2. An empty Variant — no value at all, whatever the quality bits say.
//   3. The Qualifier projection.
//
// Note the step-2 predicate is `value.is_null()` — the absence of a Variant —
// not `DataValue::is_null()`, which additionally requires a zero Qualifier and
// so misses the common case of a value-less item whose quality bits already say
// offline or misconfigured.
//
// Only the code is taken, not the limit bits `ToStatus()` also derives: both
// `scada::DataValue` and `opcua::DataValue` store a bare `StatusCode`, and the
// binary codec writes `Status{status_code}`, so a DataValue has nowhere to
// carry the low limit bits in the first place.
inline StatusCode ReportedStatusCode(const DataValue& data_value) noexcept {
  if (!IsGood(data_value.status_code))
    return data_value.status_code;
  if (data_value.value.is_null())
    return StatusCode::Bad_WaitingForInitialData;
  return data_value.qualifier.ToStatus().code();
}

inline bool IsUpdate(const DataValue& current_data, const DataValue& new_data) {
  return scada::IsNull(current_data.source_timestamp) ||
         (current_data.source_timestamp < new_data.source_timestamp) ||
         (current_data.source_timestamp == new_data.source_timestamp &&
          current_data.server_timestamp <= new_data.server_timestamp);
}

inline std::ostream& operator<<(std::ostream& stream, const DataValue& v) {
  return stream << "{value: " << v.value << ", qualifier: " << v.qualifier
                << ", source_timestamp: " << v.source_timestamp
                << ", server_timestamp: " << v.server_timestamp
                << ", status_code: " << v.status_code << "}";
}

}  // namespace scada
