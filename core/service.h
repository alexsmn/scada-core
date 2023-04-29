#pragma once

#include "base/struct_writer.h"
#include "core/node_id.h"

#include <memory>

namespace scada {

struct ServiceContext;

using ServiceContextPtr = std::shared_ptr<const ServiceContext>;

struct ServiceContext {
  static const ServiceContextPtr& default_instance() { return default_ptr; }

  NodeId user_id;
  std::vector<std::string> locale_ids;

  inline static const ServiceContextPtr default_ptr =
      std::make_shared<ServiceContext>();
};

inline std::ostream& operator<<(std::ostream& stream,
                                const ServiceContext& context) {
  StructWriter{stream}
      .AddField("user_id", ToString(context.user_id))
      .AddField("locale_ids", context.locale_ids);
  return stream;
}

}  // namespace scada
