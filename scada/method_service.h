#pragma once

#include "base/awaitable.h"
#include "scada/node_id.h"
#include "scada/status.h"
#include "scada/variant.h"

#include <vector>

namespace scada {

class MethodService {
 public:
  virtual ~MethodService() = default;

  virtual Awaitable<Status> Call(NodeId node_id,
                                 NodeId method_id,
                                 std::vector<Variant> arguments,
                                 NodeId user_id) = 0;
};

}  // namespace scada
