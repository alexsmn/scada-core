#pragma once

#include "core/node_id.h"

namespace scada {

struct ServiceContext {
  NodeId user_id;
  std::vector<std::string> locale_ids;
};

}  // namespace scada
