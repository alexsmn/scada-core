#include "remote/history_stub.h"

#include "base/any_executor_dispatch.h"
#include "base/awaitable.h"
#include "base/check.h"
#include "base/time/time_wire_codec.h"
#include "remote/message_sender.h"
#include "remote/protocol.h"
#include "remote/protocol_utils.h"
#include "scada/history_service.h"
#include "scada/locale_negotiation.h"
#include "scada/node_id_log.h"

#include "base/debug_util.h"

HistoryStub::HistoryStub(scada::HistoryService& service,
                         std::weak_ptr<MessageSender> sender,
                         AnyExecutor executor,
                         std::vector<std::string> locale_ids,
                         Tracer& tracer)
    : service_{service},
      sender_{std::move(sender)},
      executor_{std::move(executor)},
      tracer_{tracer},
      locale_ids_{std::move(locale_ids)} {}

HistoryStub::~HistoryStub() {
  // Release continuation points.
  if (!continuation_points_.empty()) {
    for (auto& [continuation_point, details] : continuation_points_) {
      details.release_continuation_point = true;
      details.continuation_point = std::move(continuation_point);
      auto& service = service_;
      CoSpawn(executor_,
              [&service,
               details = std::move(details)]() mutable -> Awaitable<void> {
                auto result =
                    co_await service.HistoryReadRaw(std::move(details));
                if (result.ok()) {
                  scada::base::Check(result->values.empty());
                  scada::base::Check(result->continuation_point.empty());
                }
              });
    }
  }
}

void HistoryStub::OnRequestReceived(const protocol::Request& request) {
  if (request.has_history_read_raw())
    OnHistoryReadRaw(request);
  if (request.has_history_read_events())
    OnHistoryReadEvents(request);
}

void HistoryStub::OnHistoryReadRaw(const protocol::Request& request) {
  auto request_id = request.request_id();
  auto& history_read_raw = request.history_read_raw();

  auto continuation_point =
      ConvertTo<scada::ByteString>(history_read_raw.continuation_point());

  scada::HistoryReadRawDetails details;

  bool ignore_params = false;
  if (!continuation_point.empty()) {
    auto i = continuation_points_.find(continuation_point);
    if (i != continuation_points_.end()) {
      details = std::move(i->second);
      continuation_points_.erase(i);
      ignore_params = true;
    }
  }

  details.release_continuation_point =
      history_read_raw.release_continuation_point();
  details.continuation_point = std::move(continuation_point);

  if (!ignore_params) {
    details.node_id = ConvertTo<scada::NodeId>(history_read_raw.node_id());
    details.from =
        history_read_raw.from_time()
            ? scada::base::DecodeWireTime(history_read_raw.from_time())
            : scada::Time();
    details.to = history_read_raw.to_time()
                     ? scada::base::DecodeWireTime(history_read_raw.to_time())
                     : scada::Time();
    details.max_count = history_read_raw.max_count();
    details.aggregation = history_read_raw.has_aggregate_filter()
                              ? ConvertTo<scada::AggregateFilter>(
                                    history_read_raw.aggregate_filter())
                              : scada::AggregateFilter{};
  }

  LOG_INFO(logger_) << "History read raw" << LOG_TAG("RequestId", request_id)
                    << LOG_TAG("NodeId", NodeIdToLogString(details.node_id));
  auto self = shared_from_this();
  CoSpawn(executor_,
          [self, request_id, trace_id = request.trace_id(),
           details = std::move(details)]() mutable -> Awaitable<void> {
            co_await self->OnHistoryReadRawAsync(
                request_id, std::move(trace_id), std::move(details));
          });
}

void HistoryStub::OnHistoryReadEvents(const protocol::Request& request) {
  auto request_id = request.request_id();
  auto& history_read_events = request.history_read_events();
  const auto node_id = ConvertTo<scada::NodeId>(history_read_events.node_id());
  auto from = history_read_events.from_time()
                  ? scada::base::DecodeWireTime(history_read_events.from_time())
                  : scada::Time();
  auto to = history_read_events.to_time()
                ? scada::base::DecodeWireTime(history_read_events.to_time())
                : scada::Time();
  scada::EventFilter filter;
  if (history_read_events.has_filter())
    Convert(history_read_events.filter(), filter);

  LOG_INFO(logger_) << "History read events" << LOG_TAG("RequestId", request_id)
                    << LOG_TAG("NodeId", NodeIdToLogString(node_id));
  auto self = shared_from_this();
  CoSpawn(executor_,
          [self, request_id, trace_id = request.trace_id(),
           node_id = std::move(node_id), from, to,
           filter = std::move(filter)]() mutable -> Awaitable<void> {
            co_await self->OnHistoryReadEventsAsync(
                request_id, std::move(trace_id), std::move(node_id), from, to,
                std::move(filter));
          });
}

Awaitable<void> HistoryStub::OnHistoryReadRawAsync(
    unsigned request_id,
    std::string trace_id,
    scada::HistoryReadRawDetails details) {
  auto span = tracer_.StartSpan("scada.grpc/HistoryReadRaw",
                                TraceSpanKind::kServer, trace_id);
  span.SetAttribute("scada.node_id", details.node_id.ToString());

  auto result = co_await service_.HistoryReadRaw(details);

  LOG_INFO(logger_) << "History read raw completed"
                    << LOG_TAG("RequestId", request_id)
                    << LOG_TAG("Status", ToString(result.status()))
                    << LOG_TAG("ValueCount",
                               ToString(result.ok() ? result->values.size()
                                                    : size_t{0}));

  if (result.ok() && !result->continuation_point.empty())
    continuation_points_.emplace(result->continuation_point, details);

  protocol::Message message;
  auto& response = *message.add_responses();
  response.set_request_id(request_id);
  Convert(result.status(), *response.mutable_status());
  if (result.ok()) {
    if (!result->values.empty()) {
      Convert(std::move(result->values),
              *response.mutable_history_read_raw_result()->mutable_value());
    }
    if (!result->continuation_point.empty()) {
      Convert(std::move(result->continuation_point),
              *response.mutable_history_read_raw_result()
                   ->mutable_continuation_point());
    }
  }

  if (auto locked_sender = sender_.lock())
    locked_sender->Send(message);
}

Awaitable<void> HistoryStub::OnHistoryReadEventsAsync(
    unsigned request_id,
    std::string trace_id,
    scada::NodeId node_id,
    scada::Time from,
    scada::Time to,
    scada::EventFilter filter) {
  auto span = tracer_.StartSpan("scada.grpc/HistoryReadEvents",
                                TraceSpanKind::kServer, trace_id);
  span.SetAttribute("scada.node_id", node_id.ToString());

  auto result = co_await service_.HistoryReadEvents(std::move(node_id), from,
                                                    to, std::move(filter));

  LOG_INFO(logger_) << "History read events completed"
                    << LOG_TAG("RequestId", request_id)
                    << LOG_TAG("Status", ToString(result.status()))
                    << LOG_TAG("EventCount",
                               result.ok() ? result->events.size() : size_t{0});

  protocol::Message message;
  auto& response = *message.add_responses();
  response.set_request_id(request_id);
  Convert(result.status(), *response.mutable_status());
  if (result.ok() && !result->events.empty()) {
    // A stored message carries every language the server could say it in
    // when the event was produced; this session gets one of them. A message
    // stored before events were multi-language resolves to itself.
    for (scada::Event& event : result->events) {
      event.message = scada::ResolveLocalizedText(event.message, locale_ids_);
      // SourceName gets the same treatment: it is the source node's
      // DisplayName as it was when the event was produced, carried with every
      // language that name had. The wire field is a plain string (Part 5
      // §6.4.2), so it must be one language by the time Convert projects it.
      //
      // And "one language" has to be enforced separately, because resolving
      // against a session that asked for "mul" correctly returns the PACKED
      // value — which `Convert` would then put on the wire as raw JSON, since
      // the proto's SourceName has no locale field beside it the way
      // `message_locale` does. `message` is safe for exactly that reason and
      // SourceName is not. Only a peer that asked for the private tier tag can
      // be relied on to unpack it again, and nothing on this protocol asks for
      // that yet — so today this always resolves down to one language. Found
      // while fixing backlog 819; the archive is downstream of this, so a
      // packed value escaping here would be stored rather than merely shown.
      event.source_name =
          scada::ResolveLocalizedText(event.source_name, locale_ids_);
      if (!scada::RequestsPackedStrings(locale_ids_)) {
        event.source_name = scada::ResolveLocalizedText(event.source_name, {});
      }
    }
    Convert(std::move(result->events),
            *response.mutable_history_read_events_result()->mutable_event());
  }

  if (auto locked_sender = sender_.lock())
    locked_sender->Send(message);
}
