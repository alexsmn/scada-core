#include "scada/service_context.h"

#include "base/struct_writer.h"

#include "base/debug_util-inl.h"

namespace scada {

struct ServiceContext::Rep {
  TraceId trace_id = 0;
  NodeId user_id;
  std::vector<std::string> locale_ids;
};

// static
const std::shared_ptr<const ServiceContext::Rep> ServiceContext::kDefaultRep =
    std::make_shared<ServiceContext::Rep>();

ServiceContext::ServiceContext(const std::shared_ptr<const Rep>& rep)
    : rep_{rep} {}

const scada::NodeId& ServiceContext::user_id() const {
  return rep_->user_id;
}

ServiceContext ServiceContext::with_user_id(
    const scada::NodeId& user_id) const {
  Rep rep = *rep_;
  rep.user_id = user_id;
  return ServiceContext{std::make_shared<Rep>(std::move(rep))};
}

std::ostream& operator<<(std::ostream& stream, const ServiceContext& context) {
  StructWriter{stream}
      .AddField("trace_id", ToString(context.rep_->trace_id))
      .AddField("user_id", ToString(context.rep_->user_id))
      .AddField("locale_ids", context.rep_->locale_ids);
  return stream;
}

}  // namespace scada
