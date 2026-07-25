#pragma once

#include "base/awaitable.h"
#include "scada/co_result.h"
#include "scada/node_id.h"
#include "scada/service_context.h"
#include "scada/status.h"
#include "scada/status_or.h"
#include "scada/variant.h"

#include <cstddef>
#include <ostream>
#include <utility>
#include <vector>

namespace scada {

// The payload of a successful Call. Operation-level failure is reported by the
// enclosing StatusOr, not by a field here — the convention `HistoryReadRawResult`
// states — so a value being present already means the method succeeded.
//
// The wire's `inputArgumentResults` is deliberately not mirrored: OPC UA Part 4
// §5.11.2 populates it only alongside Bad_InvalidArgument, which arrives here as
// a StatusOr error with no value, so a field here could never be reached.
struct CallResult {
  std::vector<Variant> output_arguments;
};

// Needed so `StatusOr<CallResult>` is streamable (status_or.h streams the
// contained value), which is what makes a failing EXPECT_EQ readable.
inline std::ostream& operator<<(std::ostream& stream, const CallResult& value) {
  stream << "{output_arguments: [";
  for (std::size_t index = 0; index < value.output_arguments.size(); ++index) {
    if (index != 0) {
      stream << ", ";
    }
    stream << value.output_arguments[index];
  }
  return stream << "]}";
}

// Callable operations on a node, invoked with the OPC UA Call service.
class MethodService {
 public:
  virtual ~MethodService() = default;

  virtual CoStatusOr<CallResult> Call(NodeId node_id,
                                      NodeId method_id,
                                      std::vector<Variant> arguments,
                                      ServiceContext context) = 0;
};

// Builds a Call result from a runtime-valued status. Use this at every leaf
// that computes a status rather than constructing StatusOr directly:
// `StatusOr<T>{status}` PANICS when `status` is ok (status_or.h
// CheckErrorStatusIsBad), and that is a runtime failure on the success path,
// not a compile error — so `co_return SomeStatus()` looks fine and crashes.
inline StatusOr<CallResult> MakeCallResult(Status status,
                                           std::vector<Variant> outputs = {}) {
  if (!status) {
    return StatusOr<CallResult>{std::move(status)};
  }
  return StatusOr<CallResult>{CallResult{std::move(outputs)}};
}

}  // namespace scada
