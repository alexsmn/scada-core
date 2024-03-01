#include "scada/service_context.h"

#include "base/struct_writer.h"

#include "base/debug_util-inl.h"

namespace scada {

std::ostream& operator<<(std::ostream& stream, const ServiceContext& context) {
  StructWriter{stream}
      .AddField("trace_id", ToString(context.trace_id))
      .AddField("user_id", ToString(context.user_id))
      .AddField("locale_ids", context.locale_ids);
  return stream;
}

}  // namespace scada
