#pragma once

#include "base/promise.h"
#include "core/method_service.h"

namespace scada {

inline promise<Status> Call(MethodService& method_service,
                            const NodeId& node_id,
                            const NodeId& method_id,
                            const std::vector<Variant>& arguments,
                            const scada::NodeId& user_id) {
  promise<Status> p;
  method_service.Call(
      node_id, method_id, arguments, user_id,
      [p](Status&& status) mutable { p.resolve(std::move(status)); });
  return p;
}

}  // namespace scada