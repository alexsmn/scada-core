#include "remote/history_proxy.h"

#include "scada/standard_node_ids.h"
#include "remote/message_sender.h"
#include "remote/protocol.h"
#include "remote/protocol_utils.h"

HistoryProxy::HistoryProxy() {}

void HistoryProxy::HistoryReadRaw(
    const scada::HistoryReadRawDetails& details,
    const scada::HistoryReadRawCallback& callback) {
  if (!sender_) {
    assert(false);
    return;
  }

  protocol::Request request;
  auto& history_read_raw = *request.mutable_history_read_raw();
  Convert(details.node_id, *history_read_raw.mutable_node_id());
  if (!details.from.is_null())
    history_read_raw.set_from_time(details.from.ToInternalValue());
  if (!details.to.is_null())
    history_read_raw.set_to_time(details.to.ToInternalValue());
  if (details.max_count != 0)
    history_read_raw.set_max_count(details.max_count);
  if (!details.aggregation.is_null())
    Convert(details.aggregation, *history_read_raw.mutable_aggregate_filter());
  if (details.release_continuation_point)
    history_read_raw.set_release_continuation_point(true);
  if (!details.continuation_point.empty()) {
    Convert(details.continuation_point,
            *history_read_raw.mutable_continuation_point());
  }

  sender_->Request(request, [this,
                             callback](const protocol::Response& response) {
    if (!callback)
      return;

    callback({ConvertTo<scada::Status>(response.status()),
              ConvertTo<std::vector<scada::DataValue>>(
                  response.history_read_raw_result().value()),
              ConvertTo<scada::ByteString>(
                  response.history_read_raw_result().continuation_point())});
  });
}

void HistoryProxy::HistoryReadEvents(
    const scada::NodeId& node_id,
    base::Time from,
    base::Time to,
    const scada::EventFilter& filter,
    const scada::HistoryReadEventsCallback& callback) {
  if (!sender_) {
    assert(false);
    return;
  }

  protocol::Request request;
  auto& history_read_events = *request.mutable_history_read_events();
  Convert(node_id, *history_read_events.mutable_node_id());
  if (!from.is_null())
    history_read_events.set_from_time(from.ToInternalValue());
  if (!to.is_null())
    history_read_events.set_to_time(to.ToInternalValue());
  Convert(filter, *history_read_events.mutable_filter());

  sender_->Request(
      request, [this, callback](const protocol::Response& response) {
        if (!callback)
          return;

        callback(ConvertTo<scada::Status>(response.status()),
                 ConvertTo<std::vector<scada::Event>>(
                     response.history_read_events_result().event()));
      });
}

void HistoryProxy::OnChannelOpened(MessageSender& sender) {
  sender_ = &sender;
}

void HistoryProxy::OnChannelClosed() {
  sender_ = nullptr;
}
