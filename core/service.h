#pragma once

#include "base/struct_writer.h"
#include "core/node_id.h"

#include <memory>

namespace scada {

struct ServiceContext {
  static const std::shared_ptr<const ServiceContext>& default_instance() {
    static auto instance = std::make_shared<const ServiceContext>();
    return instance;
  }

  NodeId user_id;
  std::vector<std::string> locale_ids;
};

inline std::ostream& operator<<(std::ostream& stream,
                                const ServiceContext& context) {
  StructWriter{stream}
      .AddField("user_id", ToString(context.user_id))
      .AddField("locale_ids", context.locale_ids);
  return stream;
}

}  // namespace scada
