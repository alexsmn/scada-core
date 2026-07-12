#include "remote/history_proxy.h"

#include "base/time/time_wire_codec.h"
#include "remote/message_sender.h"
#include "remote/protocol.h"
#include "remote/protocol_utils.h"
#include "scada/callback_awaitable.h"
#include "scada/standard_node_ids.h"

HistoryProxy::HistoryProxy(AnyExecutor executor) : executor_{std::move(executor)} {}

Awaitable<scada::HistoryReadRawResult> HistoryProxy::HistoryReadRaw(
    scada::HistoryReadRawDetails details) {
  if (!sender_) {
    co_return scada::HistoryReadRawResult{
        .status = scada::StatusCode::Bad_Disconnected};
  }

  protocol::Request request;
  auto& history_read_raw = *request.mutable_history_read_raw();
  Convert(details.node_id, *history_read_raw.mutable_node_id());
  if (!details.from.is_null())
    history_read_raw.set_from_time(base::EncodeWireMicroseconds(details.from));
  if (!details.to.is_null())
    history_read_raw.set_to_time(base::EncodeWireMicroseconds(details.to));
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

  co_return co_await scada::AwaitCallbackValue<scada::HistoryReadRawResult>(
      executor_, [this, request = std::move(request)](auto callback) mutable {
        sender_->Request(request, [callback = std::move(callback)](
                                      const protocol::Response& response) {
          auto result = scada::HistoryReadRawResult{
              .status = ConvertTo<scada::Status>(response.status()),
              .values = ConvertTo<std::vector<scada::DataValue>>(
                  response.history_read_raw_result().value()),
              .continuation_point = ConvertTo<scada::ByteString>(
                  response.history_read_raw_result().continuation_point())};
          callback(std::move(result));
        });
      });
}

Awaitable<scada::HistoryReadEventsResult> HistoryProxy::HistoryReadEvents(
    scada::NodeId node_id,
    base::Time from,
    base::Time to,
    scada::EventFilter filter) {
  if (!sender_) {
    co_return scada::HistoryReadEventsResult{
        .status = scada::StatusCode::Bad_Disconnected};
  }

  protocol::Request request;
  auto& history_read_events = *request.mutable_history_read_events();
  Convert(node_id, *history_read_events.mutable_node_id());
  if (!from.is_null())
    history_read_events.set_from_time(base::EncodeWireMicroseconds(from));
  if (!to.is_null())
    history_read_events.set_to_time(base::EncodeWireMicroseconds(to));
  Convert(filter, *history_read_events.mutable_filter());

  co_return co_await scada::AwaitCallbackValue<scada::HistoryReadEventsResult>(
      executor_, [this, request = std::move(request)](auto callback) mutable {
        sender_->Request(
            request, [callback = std::move(callback)](
                         const protocol::Response& response) {
              auto result = scada::HistoryReadEventsResult{
                  .status = ConvertTo<scada::Status>(response.status()),
                  .events = ConvertTo<std::vector<scada::Event>>(
                      response.history_read_events_result().event())};
              callback(std::move(result));
            });
      });
}

void HistoryProxy::OnChannelOpened(MessageSender& sender) {
  sender_ = &sender;
}

void HistoryProxy::OnChannelClosed() {
  sender_ = nullptr;
}
