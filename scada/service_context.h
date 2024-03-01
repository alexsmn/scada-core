#pragma once

#include "metrics/trace_id.h"
#include "scada/node_id.h"

#include <memory>
#include <ostream>

namespace scada {

struct ServiceContext;

using ServiceContextPtr = std::shared_ptr<const ServiceContext>;

struct ServiceContext {
  static const ServiceContextPtr& default_instance() { return default_ptr; }

  static ServiceContextPtr ForUser(const scada::NodeId& user_id) {
    return std::make_shared<ServiceContext>(GenerateTraceId(), user_id,
                                            std::vector<std::string>{});
  }

  TraceId trace_id = 0;
  NodeId user_id;
  std::vector<std::string> locale_ids;

  inline static const ServiceContextPtr default_ptr =
      std::make_shared<ServiceContext>();
};

std::ostream& operator<<(std::ostream& stream, const ServiceContext& context);

}  // namespace scada
