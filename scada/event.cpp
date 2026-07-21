#include "scada/event.h"

#include "base/struct_writer.h"

namespace scada {

std::ostream& operator<<(std::ostream& stream, const Event& event) {
  StructWriter{stream}
      .AddField("event_type_id", event.event_type_id)
      .AddField("event_id", event.event_id)
      .AddField("time", event.time)
      .AddField("receive_time", event.receive_time)
      .AddField("source_node_id", event.source_node_id)
      .AddField("user_id", event.user_id)
      .AddField("value", event.value)
      .AddField("message", event.message)
      .AddField("acked", event.acked)
      .AddField("acknowledged_user_id", event.acknowledged_user_id)
      .AddField("acknowledged_time", event.acknowledged_time);
  return stream;
}

}  // namespace scada
