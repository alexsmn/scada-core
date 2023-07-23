#include "remote/history_stub.h"

#include "base/executor.h"
#include "scada/history_service.h"
#include "model/node_id_util.h"
#include "remote/message_sender.h"
#include "remote/protocol.h"
#include "remote/protocol_utils.h"

#include "base/debug_util-inl.h"

#include <boost/asio/io_context.hpp>

HistoryStub::HistoryStub(scada::HistoryService& service,
                         std::weak_ptr<MessageSender> sender,
                         std::shared_ptr<Executor> executor)
    : service_{service},
      sender_{std::move(sender)},
      executor_{std::move(executor)} {}

HistoryStub::~HistoryStub() {
  // Release continuation points.
  if (!continuation_points_.empty()) {
    const scada::HistoryReadRawCallback callback =
        [](scada::HistoryReadRawResult&& result) {
          assert(result.values.empty());
          assert(result.continuation_point.empty());
        };

    for (auto& [continuation_point, details] : continuation_points_) {
      details.release_continuation_point = true;
      details.continuation_point = std::move(continuation_point);
      service_.HistoryReadRaw(details, callback);
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
            ? base::Time::FromInternalValue(history_read_raw.from_time())
            : base::Time();
    details.to = history_read_raw.to_time()
                     ? base::Time::FromInternalValue(history_read_raw.to_time())
                     : base::Time();
    details.max_count = history_read_raw.max_count();
    details.aggregation = history_read_raw.has_aggregate_filter()
                              ? ConvertTo<scada::AggregateFilter>(
                                    history_read_raw.aggregate_filter())
                              : scada::AggregateFilter{};
  }

  LOG_INFO(logger_) << "History read raw" << LOG_TAG("RequestId", request_id)
                    << LOG_TAG("NodeId", NodeIdToScadaString(details.node_id));

  service_.HistoryReadRaw(
      details,
      BindExecutor(executor_, [request_id, details, ref = shared_from_this(),
                               this](scada::HistoryReadRawResult result) {
        LOG_INFO(logger_) << "History read raw completed"
                          << LOG_TAG("RequestId", request_id)
                          << LOG_TAG("Status", ToString(result.status))
                          << LOG_TAG("ValueCount",
                                     ToString(result.values.size()));

        if (!result.continuation_point.empty())
          continuation_points_.emplace(result.continuation_point, details);

        protocol::Message message;
        auto& response = *message.add_responses();
        response.set_request_id(request_id);
        Convert(result.status, *response.mutable_status());
        if (!result.values.empty()) {
          Convert(std::move(result.values),
                  *response.mutable_history_read_raw_result()->mutable_value());
        }
        if (!result.continuation_point.empty()) {
          Convert(std::move(result.continuation_point),
                  *response.mutable_history_read_raw_result()
                       ->mutable_continuation_point());
        }

        if (auto locked_sender = sender_.lock())
          locked_sender->Send(message);
      }));
}

void HistoryStub::OnHistoryReadEvents(const protocol::Request& request) {
  auto request_id = request.request_id();
  auto& history_read_events = request.history_read_events();
  const auto node_id = ConvertTo<scada::NodeId>(history_read_events.node_id());
  auto from =
      history_read_events.from_time()
          ? base::Time::FromInternalValue(history_read_events.from_time())
          : base::Time();
  auto to = history_read_events.to_time()
                ? base::Time::FromInternalValue(history_read_events.to_time())
                : base::Time();
  scada::EventFilter filter;
  if (history_read_events.has_filter())
    Convert(history_read_events.filter(), filter);

  LOG_INFO(logger_) << "History read events" << LOG_TAG("RequestId", request_id)
                    << LOG_TAG("NodeId", NodeIdToScadaString(node_id));

  service_.HistoryReadEvents(
      node_id, from, to, filter,
      BindExecutor(executor_, [request_id, ref = shared_from_this(), this](
                                  scada::Status status,
                                  std::vector<scada::Event> events) {
        LOG_INFO(logger_) << "History read events completed"
                          << LOG_TAG("RequestId", request_id)
                          << LOG_TAG("Status", ToString(status))
                          << LOG_TAG("EventCount", events.size());

        protocol::Message message;
        auto& response = *message.add_responses();
        response.set_request_id(request_id);
        Convert(status, *response.mutable_status());
        if (!events.empty()) {
          Convert(
              std::move(events),
              *response.mutable_history_read_events_result()->mutable_event());
        }

        if (auto locked_sender = sender_.lock())
          locked_sender->Send(message);
      }));
}
